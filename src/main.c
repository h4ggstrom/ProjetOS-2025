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
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vfs_function.h>
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

        for (int j = 0; args[j] != NULL; j++)
        {
            printf("args[%d] = %s\t", j, args[j]);
        }
        printf("\n");

        if (args[0] == NULL)
        {
            continue;
        }
        else if (strcmp(args[0], "help") == 0)
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
            init_partition(&fs, "image.img", 524288, 16384);
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
            if (args[1] == 0)
            {
                printf("Il faut indiquer un nom de fichier\n");
            }
            else
            {
                uint32_t new_file = create_file(&fs, args[1],
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
        }
        else if (strcmp(args[0], "create_directory") == 0)
        {
            if (args[1] == 0)
            {
                printf("Il faut indiquer un nom de repertoire\n");
            }
            else
            {
                // Créer un répertoire avec les permissions par défaut (0755)
                char *path_copy = strdup(args[1]);
                uint32_t dir_inode = create_directory(&fs, path_copy, 0755);
                if (dir_inode == (uint32_t)-1)
                {
                    printf("Échec de la création du répertoire\n");
                }
                else
                {
                    printf("Répertoire créé avec succès (inode %u)\n", dir_inode);
                }
                continue;
            }
        }
        else if (strcmp(args[0], "ls") == 0)
        {
            bool longList;
            if (!args[2] == NULL)
            {
                char *joker = strdup(args[2]);
                if (strcmp(joker, "-l") == 0)
                { // Si joker == "-l"
                    longList = true;
                }
                else
                {
                    printf("\"%s\" n'est pas une option valide (essayez -l)\n", joker);
                }
            }
            if (args[1] == 0)
            {
                list_directory(&fs, "/", 0);
            }
            else
            {
                list_directory(&fs, args[1], longList);
            }
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
