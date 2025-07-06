<<<<<<< HEAD
# Validateur de Sudoku Multi-threadé

Un programme en C qui valide des grilles de Sudoku 9x9 en utilisant des threads pour améliorer les performances.

## Description

Ce projet utilise 11 threads pour vérifier simultanément :
- 1 thread pour valider les lignes
- 1 thread pour valider les colonnes  
- 9 threads pour valider chaque sous-grille 3x3

## Utilisation

```bash
./sudoku_validator fichier.txt
```

## Format du fichier

Le fichier doit contenir une grille Sudoku 9x9 :

```
6 2 4 5 3 9 1 8 7
5 1 9 7 2 8 6 3 4
8 3 7 6 1 4 2 9 5
1 4 3 8 6 5 7 2 9
9 5 8 2 4 7 3 6 1
7 6 2 3 9 1 4 5 8
3 7 1 9 5 6 8 4 2
4 9 6 1 8 2 5 7 3
2 8 5 4 7 3 9 1 6
```

## Types d'erreurs détectées

- Taille de grille incorrecte
- Caractères spéciaux ou non-entiers
- Doublons dans les lignes
- Doublons dans les colonnes
- Doublons dans les sous-grilles 3x3

## Exemple de sortie

```
=== Validation de Sudoku - Fichier: test.txt ===

--- Validation de la grille 1 ---
Grille 1: Bravo! Votre Sudoku est valide!

--- Validation de la grille 2 ---
Grille 2: la case 3,4 contient un caractère spécial non admis
Grille 2: Il y a un doublon dans une sous-grille 3 x 3 (sous-grille 1)

=== Résumé ===
Total de grilles trouvées: 2
Grilles valides: 1
Grilles invalides: 1
```

## Structure du projet

```
├── src/
│   ├── sudoku_validator.c
│   └── sudoku_validator.h
├── Makefile
├── test.txt
└── README.md
```

## Tests

```bash
make test
```

## Nettoyage

```bash
make clean
```

## Prérequis

- GCC
- pthread library
- Linux/Unix

## Auteur
Massil Zadoud
Projet réalisé dans le cadre du cours INF3173 à l'UQAM.
=======

