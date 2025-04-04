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
#include "../include/permissions.h"
#include "../include/links.h"
// Constantes
#define MAX_LINE 1024
#define MAX_ARGS 64

// Prototype
void display_menu();
void build_partition(FileSystem *fs);
void build_demo();
void execute_cd(char **args);
int is_string_numeric(const char *str);
void display_help();

/**
 * @brief Point d'entrée principal du shell interactif
 *
 * @return int Code de sortie :
 *             - EXIT_SUCCESS (0) en cas de succès
 *             - EXIT_FAILURE (1) en cas d'erreur critique
 *
 * @details Fonctionnement du shell :
 *
 * ### Commandes internes :
 * - `exit` : Quitte le shell
 * - `cd [dir]` : Change le répertoire courant
 * - `display_menu` : Affiche le menu des options
 * - `build_demo` : Construit l'arborescence de démonstration
 *
 * ### Options numériques (1-10) :
 * 1. Créer un fichier
 * 2. Supprimer un fichier
 * 3. Copier un fichier
 * 4. Déplacer un fichier
 * 5. Créer un répertoire
 * 6. Créer un lien symbolique
 * 7. Créer un lien dur
 * 8. Modifier les permissions
 * 9. Lister un répertoire
 * 10. Quitter
 *
 * ### Commandes externes :
 * Exécute n'importe quelle commande système disponible dans $PATH
 *
 * @note Le shell gère jusqu'à MAX_ARGS arguments par commande
 * @warning Les commandes externes sont exécutées dans un processus fils
 *
 * @see execute_cd(), display_menu(), build_demo() pour les fonctions internes
 * @see create_file(), delete_file(), mycp() pour les opérations sur fichiers
 */
int main()
{
    FileSystem fs;
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    char *token;
    int status;
    pid_t pid;

    while (1)
    {
        // Afficher le prompt
        printf("Tapez \"help\" pour afficher l'aide\nmyshell> ");
        fflush(stdout);

        // Lire la ligne de commande
        if (!fgets(line, MAX_LINE, stdin))
        {
            break; // Sortir si EOF (Ctrl+D)
        }

        // Supprimer le saut de ligne
        line[strcspn(line, "\n")] = '\0';

        // Tokenizer la ligne pour obtenir les arguments
        int i = 0;
        token = strtok(line, " ");
        while (token != NULL && i < MAX_ARGS - 1)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Terminer le tableau avec NULL

        if (strcmp(args[0], "help") == 0)
        {
            display_help();
            continue;
        }
        else if (strcmp(args[0], "test") == 0)
        {
            printf("test");
            continue;
        }
        else if (strcmp(args[0], "build") == 0)
        {
            printf("Début du Build de la partition\n");
            init_partition(&fs, "image.img", 50000, 4096);
            printf("Build de la partition terminé\n");
            continue;
        }
        else if (strcmp(args[0], "load") == 0)
        {
            printf("Test chargement de la partition\n");
            load_partition(&fs, "image.img");
            printf("Chargement de la partition terminé\n");
            continue;
        }
        else if (strcmp(args[0], "create_file") == 0)
        {
            uint32_t new_file = create_file(&fs,"/test.txt",
                                            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (new_file != (uint32_t)-1)
            {
                printf("Fichier créé avec inode %u\n", new_file);
            }
            else
            {
                printf("Échec de la création du fichier\n");
            };
            continue;
        }
        /*
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
                    */
    }
    return EXIT_SUCCESS;
}

/**
 * @brief vérifie si la chaine de caractères est un nombre
 * @param str string à vérifier
 */
int is_string_numeric(const char *str)
{
    if (str == NULL)
        return 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Affiche l'aide pour toutes les commandes disponibles
 *
 * @details Liste toutes les commandes internes et leur fonctionnalité
 */
void display_help()
{
    printf("\nAide du shell - Commandes disponibles:\n\n");
    printf("Commandes internes:\n");
    printf("  %-20s %s\n", "exit", "Quitter le shell");
    printf("  %-20s %s\n", "cd [dir]", "Changer de répertoire");
    printf("  %-20s %s\n", "build_demo", "Construire l'arborescence de démo");
    printf("  %-20s %s\n", "help", "Afficher cette aide");
}

void build_partition(FileSystem *fs)
{
    printf("Début du Build de la partition");
    init_partition(fs, "image.img", 5000000, 1024);
    printf("Build de la partition terminé");
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