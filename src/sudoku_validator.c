#include "sudoku_validator.h"

// Variables globales
int sudoku[GRID_SIZE][GRID_SIZE];
int result[NUM_THREADS]; // Tableau des résultats pour chaque thread
int error_found = 0;
char error_message[256];
error_info current_error = {{{0}}, 0, 0};

// Fonction pour valider toutes les lignes
void* validate_all_rows(void* param) {
    parameters* data = (parameters*) param;
    int thread_id = data->thread_id;
    
    for (int row = 0; row < GRID_SIZE; row++) {
        int numbers[GRID_SIZE + 1] = {0};
        for (int col = 0; col < GRID_SIZE; col++) {
            int num = sudoku[row][col];
            if (numbers[num] == 1) {
                result[thread_id] = 0;
                free(param);
                return NULL;
            }
            numbers[num] = 1;
        }
    }
    
    result[thread_id] = 1;
    free(param);
    return NULL;
}

// Fonction pour valider toutes les colonnes
void* validate_all_columns(void* param) {
    parameters* data = (parameters*) param;
    int thread_id = data->thread_id;
    
    for (int col = 0; col < GRID_SIZE; col++) {
        int numbers[GRID_SIZE + 1] = {0};
        for (int row = 0; row < GRID_SIZE; row++) {
            int num = sudoku[row][col];
            if (numbers[num] == 1) {
                result[thread_id] = 0;
                free(param);
                return NULL;
            }
            numbers[num] = 1;
        }
    }
    
    result[thread_id] = 1;
    free(param);
    return NULL;
}

// Fonction pour valider une sous-grille 3x3
void* validate_subgrid(void* param) {
    parameters* data = (parameters*) param;
    int start_row = data->ligne;
    int start_col = data->colonne;
    int thread_id = data->thread_id;
    
    int numbers[GRID_SIZE + 1] = {0};
    
    for (int row = start_row; row < start_row + SUBGRID_SIZE; row++) {
        for (int col = start_col; col < start_col + SUBGRID_SIZE; col++) {
            int num = sudoku[row][col];
            if (num < 1 || num > 9) {
                result[thread_id] = 0;
                free(param);
                return NULL;
            }
            if (numbers[num] == 1) {
                result[thread_id] = 0;
                free(param);
                return NULL;
            }
            numbers[num] = 1;
        }
    }
    
    result[thread_id] = 1;
    free(param);
    return NULL;
}

// Fonction pour ajouter une erreur à la liste
void add_error(int error_type, int row, int col, char character) {
    if (current_error.error_count < 100) {
        current_error.errors[current_error.error_count].error_type = error_type;
        current_error.errors[current_error.error_count].row = row;
        current_error.errors[current_error.error_count].col = col;
        current_error.errors[current_error.error_count].character = character;
        current_error.error_count++;
    }
}

// Fonction pour valider le format du caractère (sans affichage)
int validate_character_silent(char c, int row, int col) {
    if (!isdigit(c)) {
        if (c == '$' || c == '%' || c == '!' || c == '@' || c == '#' || 
            c == '^' || c == '&' || c == '*' || c == '(' || c == ')') {
            add_error(1, row, col, c); // caractère spécial
        } else {
            add_error(2, row, col, c); // caractère non-entier
        }
        return 0;
    }
    
    int num = c - '0';
    if (num < 1 || num > 9) {
        add_error(2, row, col, c); // caractère non-entier
        return 0;
    }
    
    return num;
}

// Fonction pour forcer la lecture complète d'une grille même avec erreurs
int read_grid_force_complete(FILE* file, int grid_num) {
    (void)grid_num;
    char line[256];
    int row = 0;
    
    // Réinitialiser les erreurs
    current_error.error_count = 0;
    current_error.has_size_error = 0;
    
    // Lire exactement 9 lignes pour cette grille
    while (row < GRID_SIZE) {
        if (!fgets(line, sizeof(line), file)) {
            if (row == 0) {
                return -1; // Fin de fichier
            } else {
                current_error.has_size_error = 1;
                // Compléter avec des 1 pour permettre l'analyse
                for (int remaining_row = row; remaining_row < GRID_SIZE; remaining_row++) {
                    for (int col = 0; col < GRID_SIZE; col++) {
                        sudoku[remaining_row][col] = 1;
                    }
                }
                return 0;
            }
        }
        
        // Ignorer les lignes vides
        int has_content = 0;
        for (int i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
            if (line[i] != ' ' && line[i] != '\t') {
                has_content = 1;
                break;
            }
        }
        
        if (!has_content) {
            continue;
        }
        
        int col = 0;
        int i = 0;
        
        // Parcourir la ligne caractère par caractère
        while (line[i] != '\0' && line[i] != '\n') {
            if (line[i] != ' ' && line[i] != '\t') {
                if (col >= GRID_SIZE) {
                    current_error.has_size_error = 1;
                    break;
                }
                
                int num = validate_character_silent(line[i], row, col);
                if (num == 0) {
                    // Remplacer par 1 pour continuer l'analyse
                    sudoku[row][col] = 1;
                } else {
                    sudoku[row][col] = num;
                }
                col++;
            }
            i++;
        }
        
        // Compléter la ligne si elle est trop courte
        if (col != GRID_SIZE) {
            current_error.has_size_error = 1;
            for (int remaining_col = col; remaining_col < GRID_SIZE; remaining_col++) {
                sudoku[row][remaining_col] = 1;
            }
        }
        
        row++;
    }
    
    return 1; // Toujours retourner succès pour permettre l'analyse
}

// Fonction pour valider une grille avec les threads
int validate_sudoku_grid(int grid_num) {
    // Initialiser le tableau des résultats
    for (int i = 0; i < NUM_THREADS; i++) {
        result[i] = 0;
    }
    
    pthread_t threads[NUM_THREADS];
    int thread_count = 0;
    
    // Thread pour valider toutes les lignes
    parameters* row_data = (parameters*) malloc(sizeof(parameters));
    row_data->thread_id = thread_count++;
    pthread_create(&threads[0], NULL, validate_all_rows, row_data);
    
    // Thread pour valider toutes les colonnes
    parameters* col_data = (parameters*) malloc(sizeof(parameters));
    col_data->thread_id = thread_count++;
    pthread_create(&threads[1], NULL, validate_all_columns, col_data);
    
    // 9 threads pour valider chaque sous-grille 3x3
    int thread_index = 2;
    for (int i = 0; i < SUBGRID_SIZE; i++) {
        for (int j = 0; j < SUBGRID_SIZE; j++) {
            parameters* subgrid_data = (parameters*) malloc(sizeof(parameters));
            subgrid_data->ligne = i * SUBGRID_SIZE;
            subgrid_data->colonne = j * SUBGRID_SIZE;
            subgrid_data->thread_id = thread_count++;
            pthread_create(&threads[thread_index], NULL, validate_subgrid, subgrid_data);
            thread_index++;
        }
    }
    
    // Attendre que tous les threads se terminent
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Vérifier les résultats
    int sudoku_valid = 1;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (result[i] == 0) {
            sudoku_valid = 0;
            break;
        }
    }
    
    if (sudoku_valid) {
        printf("Grille %d: Bravo! Votre Sudoku est valide!\n", grid_num);
        return 1;
    } else {
        // Analyser plus précisément où sont les erreurs
        if (result[0] == 0) {
            printf("Grille %d: Il y a un doublon dans la grille 9 x 9 (lignes)\n", grid_num);
        }
        if (result[1] == 0) {
            printf("Grille %d: Il y a un doublon dans la grille 9 x 9 (colonnes)\n", grid_num);
        }
        
        // Afficher TOUTES les erreurs de sous-grilles
        for (int i = 2; i < NUM_THREADS; i++) {
            if (result[i] == 0) {
                int subgrid_num = i - 2;
                printf("Grille %d: Il y a un doublon dans une sous-grille 3 x 3 (sous-grille %d)\n", 
                       grid_num, subgrid_num + 1);
            }
        }
        return 0;
    }
}

// Fonction pour afficher le Sudoku (pour debug)
void print_sudoku(int grid_num) {
    printf("Grille %d:\n", grid_num);
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fichier_sudoku>\n", argv[0]);
        return 1;
    }
    
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", argv[1]);
        return 1;
    }
    
    int grid_num = 1;
    int total_grids = 0;
    int valid_grids = 0;
    
    printf("=== Validation de Sudoku - Fichier: %s ===\n\n", argv[1]);
    
    // Lire et valider chaque grille dans le fichier
    while (1) {
        int read_result = read_grid_force_complete(file, grid_num);
        
        if (read_result == -1) {
            // Fin de fichier atteinte
            break;
        }
        
        // Afficher la section pour chaque grille
        printf("--- Validation de la grille %d ---\n", grid_num);
        
        // Afficher toutes les erreurs de format trouvées
        if (current_error.error_count > 0) {
            for (int i = 0; i < current_error.error_count; i++) {
                if (current_error.errors[i].error_type == 1) {
                    printf("Grille %d: la case %d,%d contient un caractère spécial non admis\n", 
                           grid_num, current_error.errors[i].col+1, current_error.errors[i].row+1);
                } else if (current_error.errors[i].error_type == 2) {
                    printf("Grille %d: la case %d,%d contient un caractère non-entier\n", 
                           grid_num, current_error.errors[i].col+1, current_error.errors[i].row+1);
                }
            }
        }
        
        if (current_error.has_size_error) {
            printf("Grille %d: La taille de la grille de Sudoku devrait être 9x9\n", grid_num);
        }
        
        // Toujours valider les doublons (même avec des erreurs de format)
        int has_format_errors = (current_error.error_count > 0 || current_error.has_size_error);
        
        if (validate_sudoku_grid(grid_num)) {
            if (!has_format_errors) {
                valid_grids++;
            }
        }
        
        total_grids++;
        printf("\n");
        grid_num++;
        
        // Limiter à un nombre raisonnable de grilles
        if (grid_num > 10) {
            break;
        }
    }
    
    fclose(file);
    
    // Message de fin de traitement
    printf("Fichier terminé.\n\n");
    
    // Résumé final
    printf("=== Résumé ===\n");
    printf("Total de grilles trouvées: %d\n", total_grids);
    printf("Grilles valides: %d\n", valid_grids);
    printf("Grilles invalides: %d\n", total_grids - valid_grids);
    
    return 0;
}