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
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include "../include/partition.h"
#include "../include/file_operations.h"
#include "../include/permissions.h"
#include "../include/links.h"
//Constantes
#define MAX_LINE 1024
#define MAX_ARGS 64

//Prototype
void display_menu();
void build_demo();
void execute_cd(char **args);
int is_string_numeric(const char *str);


int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    char *token;
    int status;
    pid_t pid;

    while (1) {
        // Afficher le prompt
        printf("myshell> ");
        fflush(stdout);

        // Lire la ligne de commande
        if (!fgets(line, MAX_LINE, stdin)) {
            break; // Sortir si EOF (Ctrl+D)
        }

        // Supprimer le saut de ligne
        line[strcspn(line, "\n")] = '\0';

        // Tokenizer la ligne pour obtenir les arguments
        int i = 0;
        token = strtok(line, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Terminer le tableau avec NULL

        // Gérer les commandes internes
        if(args[0] != NULL && is_string_numeric(args[0])) {
            int num = atoi(args[0]);
            if (num >= 1 && num <= 10) {
                char path[256], target[256], content[256];
                switch (num) {
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
                continue;
            }

        }else if (strcmp(args[0], "exit") == 0) {
            break;
        }else if (strcmp(args[0], "display_menu") == 0) {
            display_menu();
            continue; // Passer à l'itération suivante sans forker
        } else if (strcmp(args[0], "cd") == 0) {
            execute_cd(args);
            continue; // Passer à l'itération suivante sans forker
        }else if (strcmp(args[0], "build_demo") == 0) {
            build_demo();
            continue; // Passer à l'itération suivante sans forker
        }

        // Forker un processus enfant
        pid = fork();
        if (pid == 0) {
            // Processus enfant
            execvp(args[0], args);
            // Si execvp retourne, c'est qu'il y a eu une erreur
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Erreur lors du fork
            perror("fork");
        } else {
            // Processus parent - attendre la fin de l'enfant
            waitpid(pid, &status, 0);
        }
    }

    return EXIT_SUCCESS;
}

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
}

void build_demo(){
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
}

void execute_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: argument attendu\n");
    } else {
        DIR* dir = opendir(args[1]);
        if (dir) {
            closedir(dir);
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        } else {
            fprintf(stderr, "cd: %s: Répertoire inexistant\n", args[1]);
        }
    }
}


int is_string_numeric(const char *str) {
    if (str == NULL) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

/*
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
}*/