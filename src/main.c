/**
Main program for creating the required directory structure and files.

This program uses the functions defined in the project to create
directories, files, and links as specified in the requirements.
The structure is created inside a "demo" directory at the root level of the project.

@author 
Robin de Angelis (%)
Alexandre Ledard (%)
Killian Treuil (%)

@version 0.1 01/04/2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/partition.h"
#include "../include/file_operations.h"
#include "../include/permissions.h"
#include "../include/links.h"

// Fonction pour afficher le menu
void display_menu() {
    printf("\nMenu des commandes principales :\n");
    printf("1. Créer un fichier\n");
    printf("2. Supprimer un fichier\n");
    printf("3. Copier un fichier\n");
    printf("4. Déplacer un fichier\n");
    printf("5. Créer un répertoire\n");
    printf("6. Créer un lien symbolique\n");
    printf("7. Créer un lien dur\n");
    printf("8. Modifier les droits d'accès d'un fichier\n");
    printf("9. Afficher le contenu d'un répertoire\n"); // Nouvelle option
    printf("10. Quitter\n");
    printf("Choisissez une option : ");
}

int main() {
    // Création des répertoires
    create_directories_recursively("demo/rep01/rep01_01");
    create_directories_recursively("demo/rep01/rep01_02");
    create_directories_recursively("demo/rep01/rep01_03");
    create_directories_recursively("demo/rep02/rep02_01/rep02_01_01/rep02_01_01_01");

    // Création des fichiers
    create_file("demo/rep01/file_test.txt", "Contenu de file_test.txt");
    create_file("demo/rep01/rep01_01/file_test.txt", "Contenu de file_test.txt");
    create_file("demo/rep01/rep01_02/file_test_link.txt", "Contenu de file_test_link.txt");
    create_file("demo/rep01/rep01_03/file_test.txt", "Contenu de file_test.txt");
    create_file("demo/rep02/file_test_lien_dur.txt", "Contenu de file_test_lien_dur.txt");
    create_file("demo/rep02/rep02_01/file_test.txt", "Contenu de file_test.txt");
    create_file("demo/rep02/rep02_01/rep02_01_01/rep02_01_01_01/file_test.txt", "Contenu de file_test.txt");

    // Création des liens symboliques et durs
    create_soft_link("demo/rep01/rep01_02/file_test_link.txt", "demo/rep01/file_test_lien_symbolique.txt");
    create_hard_link("demo/rep02/file_test_lien_dur.txt", "demo/rep02/rep02_01/file_test_lien_dur.txt");

    printf("Arborescence créée avec succès dans le dossier 'demo' !\n");

    // Menu interactif
    int choice;
    char path[256], target[256], content[256];
    while (1) {
        display_menu();
        scanf("%d", &choice);
        getchar(); // Consommer le caractère '\n' restant

        switch (choice) {
            case 1: // Créer un fichier
                printf("Entrez le chemin du fichier : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0'; // Supprimer le '\n'
                printf("Entrez le contenu du fichier (laisser vide pour aucun contenu) : ");
                fgets(content, sizeof(content), stdin);
                content[strcspn(content, "\n")] = '\0';
                create_file(path, strlen(content) > 0 ? content : NULL);
                break;

            case 2: // Supprimer un fichier
                printf("Entrez le chemin du fichier à supprimer : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0';
                delete_file(path);
                break;

            case 3: // Copier un fichier
                printf("Entrez le chemin du fichier source : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0';
                printf("Entrez le chemin du fichier destination : ");
                fgets(target, sizeof(target), stdin);
                target[strcspn(target, "\n")] = '\0';
                mycp(path, target);
                break;

            case 4: // Déplacer un fichier
                printf("Entrez le chemin du fichier source : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0';
                printf("Entrez le répertoire de destination : ");
                fgets(target, sizeof(target), stdin);
                target[strcspn(target, "\n")] = '\0';
                mymv(path, target);
                break;

            case 5: // Créer un répertoire
                printf("Entrez le chemin du répertoire à créer : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0';
                create_directory(path);
                break;

            case 6: // Créer un lien symbolique
                printf("Entrez le chemin de la cible : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0';
                printf("Entrez le chemin du lien symbolique : ");
                fgets(target, sizeof(target), stdin);
                target[strcspn(target, "\n")] = '\0';
                create_soft_link(path, target);
                break;

            case 7: // Créer un lien dur
                printf("Entrez le chemin de la cible : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0';
                printf("Entrez le chemin du lien dur : ");
                fgets(target, sizeof(target), stdin);
                target[strcspn(target, "\n")] = '\0';
                create_hard_link(path, target);
                break;

            case 8: // Modifier les droits d'accès d'un fichier
                printf("Entrez le chemin du fichier ou répertoire : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0'; // Supprimer le '\n'
                printf("Entrez les nouveaux droits (en octal, ex: 0644) : ");
                unsigned int mode;
                scanf("%o", &mode); // Lire les droits en octal
                getchar(); // Consommer le caractère '\n' restant
                if (change_permissions(path, mode) == 0) {
                    printf("Droits modifiés avec succès pour : %s\n", path);
                } else {
                    printf("Échec de la modification des droits pour : %s\n", path);
                }
                break;

            case 9: // Afficher le contenu d'un répertoire
                printf("Entrez le chemin du répertoire (laisser vide pour le répertoire courant) : ");
                fgets(path, sizeof(path), stdin);
                path[strcspn(path, "\n")] = '\0'; // Supprimer le '\n'
                if (print_directory_content(strlen(path) > 0 ? path : NULL) == -1) {
                    printf("Erreur lors de l'affichage du contenu du répertoire.\n");
                }
                break;

            case 10: // Quitter
                printf("Au revoir !\n");
                exit(0);

            default:
                printf("Option invalide. Veuillez réessayer.\n");
        }
    }

    return 0;
}