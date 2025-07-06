/**
 * Ce fichier contient toutes les déclarations de structures, constantes,
 * fonctions et variables globales nécessaires pour le validateur de Sudoku
 * utilisant une architecture multi-threadée avec pthreads.
 * 
 * @author Massil Zadoud
 * @date Juillet 2025
 * INF3173 - Principes des systèmes d'exploitation
 * Université du Québec à Montréal
 */

#ifndef SUDOKU_VALIDATOR_H
#define SUDOKU_VALIDATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

// Constantes
#define GRID_SIZE 9
#define SUBGRID_SIZE 3
#define NUM_THREADS 11

// Structure pour passer des paramètres aux threads
typedef struct {
    int ligne;
    int colonne;
    int thread_id;
} parameters;

// Structure pour stocker les erreurs trouvées
typedef struct {
    int error_type; // 1 = caractère spécial, 2 = caractère non-entier, 3 = taille incorrecte
    int row;
    int col;
    char character; // Le caractère problématique
} single_error;

typedef struct {
    single_error errors[100]; // Maximum 100 erreurs par grille
    int error_count;
    int has_size_error;
} error_info;

// Déclarations des fonctions
void* validate_all_rows(void* param);
void* validate_all_columns(void* param);
void* validate_subgrid(void* param);
int validate_character(char c, int row, int col, int grid_num);
int read_single_sudoku(FILE* file, int grid_num);
int validate_sudoku_grid(int grid_num);
void print_sudoku(int grid_num);

// Variables globales (à déclarer comme extern dans le .h)
extern int sudoku[GRID_SIZE][GRID_SIZE];
extern int result[NUM_THREADS];
extern int error_found;
extern char error_message[256];

#endif