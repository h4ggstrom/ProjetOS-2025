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
#include <tree.h>
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
int make_demo_directory(FileSystem *fs);

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
        else if (strcmp(args[0], "make_demo") == 0)
        {
            make_demo_directory(&fs);
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
        else if (strcmp(args[0], "getcwd") == 0)
        {
            printf("Répertoire courant: %s\n", get_current_directory(&fs));
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
        else if (strcmp(args[0], "open_file") == 0)
        {
            if (args[1] == 0)
            {
                printf("Il faut indiquer un nom de fichier\n");
            }
            else
            {
               int fd = fs_open_file(&fs,args[1],O_RDWR);
               printf("Descripteur de fichier:%d\n",fd);
               if (fd == -1) {
                perror("open_file failed\n");
            }
            else{printf("Fichier ouvert avec succees\n");}
            continue;}
        }
        else if (strcmp(args[0], "close_file") == 0)
        {
            if (args[1] == 0)
            {
                printf("Il faut indiquer un nom de fichier\n");
            }
            else
            {
               int fd = fs_close_file(&fs,fd);
               printf("Descripteur de fichier:%d\n",fd);
               if (fd == -1) {
                perror("close_file failed\n");
            }else{printf("Fichier ferme avec succee\n");}
            continue;}
        }
        else if (strcmp(args[0], "tree") == 0)
        {
            char path[MAX_PATH_LEN];
            if (args[1] == NULL)
            {
                const char *current_dir = get_current_directory(&fs);
                // Évite les doublons de "/"
                if (current_dir[0] == '/')
                {
                    snprintf(path, sizeof(path), "%s", current_dir); // Garde le chemin tel quel (déjà absolu)
                }
                else
                {
                    snprintf(path, sizeof(path), "/%s", current_dir); // Ajoute un "/" seulement si nécessaire
                }
                tree(&fs, path,MAX_PATH_DEPTH);
            }
            else if(!args[1] == NULL)
            {
                tree(&fs,args[1],MAX_PATH_DEPTH);
            }
            continue;
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
            }
            continue;
        }
        else if (strcmp(args[0], "remove_directory") == 0)
        {
            if (args[1] == NULL)
            {
                printf("Il faut indiquer un répertoire à supprimer\n");
            }
            else
            {
                if (remove_directory(&fs, args[1]) == 0)
                {
                    printf("Répertoire \"%s\" supprimé avec succès\n", args[1]);
                }
                else
                {
                    printf("Échec de la suppression du répertoire\n");
                }
            }
            continue;
        }
        else if (strcmp(args[0], "remove_file") == 0)
        {
            if (args[1] == NULL)
            {
                printf("Il faut indiquer un fichier à supprimer\n");
            }
            else
            {
                if (remove_file(&fs, args[1]) == 0)
                {
                    printf("Fichier \"%s\" supprimé avec succès\n", args[1]);
                }
                else
                {
                    printf("Échec de la suppression du fichier\n");
                }
            }
            continue;
        }
        
        else if (strcmp(args[0], "chdir") == 0)
        {
            if (args[1] == NULL)
            {
                printf("Il faut indiquer un répertoire \n");
            }
            else
            {
                if (change_directory(&fs, args[1]) == 0)
                {
                    printf("Déplacement dans le repertoire \"%s\"\n", args[1]);
                }
                else
                {
                    printf("Échec du déplacement dans le répertoire\n");
                }
            }
            continue;
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
                char path[MAX_PATH_LEN];
                const char *current_dir = get_current_directory(&fs);

                // Évite les doublons de "/"
                if (current_dir[0] == '/')
                {
                    snprintf(path, sizeof(path), "%s", current_dir); // Garde le chemin tel quel (déjà absolu)
                }
                else
                {
                    snprintf(path, sizeof(path), "/%s", current_dir); // Ajoute un "/" seulement si nécessaire
                }
                list_directory(&fs, path, 0);
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

int make_demo_directory(FileSystem *fs){

    if (!fs) return -1;
    // Création des répertoires avec permissions 0755
    if (create_directory(fs, "/home", 0755) == (uint32_t)-1) return -1;
    if (create_directory(fs, "/home/user", 0755) == (uint32_t)-1) return -1;
    if (create_directory(fs, "/home/guest", 0755) == (uint32_t)-1) return -1;
    if (create_directory(fs, "/home/user/documents", 0755) == (uint32_t)-1) return -1;
    if (create_directory(fs, "/home/user/photos", 0755) == (uint32_t)-1) return -1;
    if (create_directory(fs, "/bin", 0755) == (uint32_t)-1) return -1;
    if (create_directory(fs, "/etc", 0755) == (uint32_t)-1) return -1;

    // Création des fichiers exécutables avec permissions 0755
    if (create_file(fs, "/bin/ls", 0755) == (uint32_t)-1) return -1;
    if (create_file(fs, "/bin/sh", 0755) == (uint32_t)-1) return -1;

    // Création des fichiers de configuration avec permissions 0644
    if (create_file(fs, "/etc/config", 0644) == (uint32_t)-1) return -1;

    // Création du fichier passwd avec permissions restreintes 0600
    if (create_file(fs, "/etc/passwd", 0600) == (uint32_t)-1) return -1;

    return 0;
}