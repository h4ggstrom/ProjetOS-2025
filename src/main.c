/**
 * @file main.c
 * @brief Main program for creating the required directory structure and files.
 *
 * @details This program uses the functions defined in the project to create
directories, files, and links as specified in the requirements.
The structure is created inside a "demo" directory at the root level of the project.
 *
 * @version 0.3
 * @date 04/04/2025
 *
 * @authors
 * - Robin de Angelis (%)
 * - Alexandre Ledard (%)
 * - Killian Treuil (%)
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
#include "../include/user.h"

// Constantes
#define MAX_LINE 1024
#define MAX_ARGS 64

// Prototype
void display_menu();
void build_demo();
void execute_cd(char **args);
int is_string_numeric(const char *str);
void display_help();
int make_demo_directory(FileSystem *fs);
void initialize_default_user();

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
    // int status;
    // pid_t pid;        unused for now

    initialize_default_user();

    while (1)
    {
        // Afficher le prompt
        User *current_user = get_current_user();
        printf("\033[1;34m%s\033[0m $ ", current_user->username);
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
            printf("Tapez \"help\" pour afficher l'aide\n");
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
            init_partition(&fs, "image.img", 16777216, 16384);
            printf("Build de la partition terminé\n");
            continue;
        }
        else if (strcmp(args[0], "load") == 0)
        {
            load_partition(&fs, "image.img");
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
                if (args[1][0] != "/")
                {
                    char path[MAX_PATH_LEN];
                    const char *current_dir = get_current_directory(&fs);
                    if (current_dir[0] == '/')
                    {
                        snprintf(path, sizeof(path), "%s%s", current_dir, args[1]); // Garde le chemin tel quel (déjà absolu)
                    }
                    else
                    {
                        snprintf(path, sizeof(path), "/%s%s", current_dir, args[1]); // Ajoute un "/" seulement si nécessaire
                    }
                    uint32_t new_file = create_file(&fs, path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                    if (new_file != (uint32_t)-1)
                    {
                        printf("Fichier créé avec inode %u\n", new_file);
                    }
                    else
                    {
                        printf("Échec de la création du fichier\n");
                    }
                }
                else
                {
                    uint32_t new_file = create_file(&fs, args[1], S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                    if (new_file != (uint32_t)-1)
                    {
                        printf("Fichier créé avec inode %u\n", new_file);
                    }
                    else
                    {
                        printf("Échec de la création du fichier\n");
                    };
                }
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
                int fd = fs_open_file(&fs, args[1], O_RDWR);
                printf("Descripteur de fichier:%d\n", fd);
                if (fd == -1)
                {
                    perror("open_file failed\n");
                }
                else
                {
                    printf("Fichier ouvert avec succees\n");
                }
                continue;
            }
        }
        else if (strcmp(args[0], "close_file") == 0)
        {
            if (args[1] == 0)
            {
                printf("Il faut indiquer un nom de fichier\n");
            }
            else
            {
                int fd = fs_close_file(&fs, fd);
                printf("Descripteur de fichier:%d\n", fd);
                if (fd == -1)
                {
                    perror("close_file failed\n");
                }
                else
                {
                    printf("Fichier ferme avec succee\n");
                }
                continue;
            }
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
                tree(&fs, path, MAX_PATH_DEPTH);
            }
            else if (!args[1] == NULL)
            {
                tree(&fs, args[1], MAX_PATH_DEPTH);
            }
            continue;
        }

        else if (strcmp(args[0], "create_directory") == 0)
        {
            if (args[1] == 0)
            {
                printf("Il faut indiquer un nom de répertoire\n");
            }
            else
            {
                if (args[1][0] != "/")
                {
                    char path[MAX_PATH_LEN];
                    const char *current_dir = get_current_directory(&fs);
                    if (current_dir[0] == '/')
                    {
                        snprintf(path, sizeof(path), "%s%s", current_dir, args[1]); // Garde le chemin tel quel (déjà absolu)
                    }
                    else
                    {
                        snprintf(path, sizeof(path), "/%s%s", current_dir, args[1]); // Ajoute un "/" seulement si nécessaire
                    }
                    uint32_t new_directory = create_directory(&fs, path, 0755);
                    if (new_directory != (uint32_t)-1)
                    {
                        printf("Repertoire créé avec l'inode %u\n", new_directory);
                    }
                    else
                    {
                        printf("Échec de la création du repertoire\n");
                    }
                }
                else
                {
                    uint32_t new_directory = create_directory(&fs, args[1], 0755);
                    if (new_directory != (uint32_t)-1)
                    {
                        printf("Fichier créé avec inode %u\n", new_directory);
                    }
                    else
                    {
                        printf("Échec de la création du fichier\n");
                    };
                }
            }
            continue;
        }
        else if (strcmp(args[0], "remove_directory") == 0)
        {
            if (args[1] == 0)
            {
                printf("Il faut indiquer un nom de répertoire\n");
            }
            else
            {
                if (args[1][0] != '/')
                {
                    char path[MAX_PATH_LEN];
                    const char *current_dir = get_current_directory(&fs);
                    if (current_dir[0] == '/')
                    {
                        snprintf(path, sizeof(path), "%s/%s", current_dir, args[1]); // Garde le chemin tel quel (déjà absolu)
                    }
                    else
                    {
                        snprintf(path, sizeof(path), "/%s/%s", current_dir, args[1]); // Ajoute un "/" seulement si nécessaire
                    }
                    if (remove_directory(&fs, path) == 0)
                    {
                        printf("Répertoire \"%s\" supprimé avec succès\n", path);
                    }
                    else
                    {
                        printf("Échec de la suppression du répertoire: %s\n", path);
                    }
                }
                else
                {
                    if (remove_directory(&fs, args[1]) == 0)
                    {
                        printf("Répertoire \"%s\" supprimé avec succès\n", args[1]);
                    }
                    else
                    {
                        printf("Échec de la suppression du répertoire:\n", args[1]);
                    }
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
                if (args[1][0] != "/")
                {
                    char path[MAX_PATH_LEN];
                    const char *current_dir = get_current_directory(&fs);
                    if (current_dir[0] == '/')
                    {
                        snprintf(path, sizeof(path), "%s%s", current_dir, args[1]); // Garde le chemin tel quel (déjà absolu)
                    }
                    else
                    {
                        snprintf(path, sizeof(path), "/%s%s", current_dir, args[1]); // Ajoute un "/" seulement si nécessaire
                    }
                    if (change_directory(&fs, path) == 0)
                    {
                        printf("Déplacement dans le répertoire \"%s\"\n", path);
                    }
                    else
                    {
                        printf("Échec du déplacement dans le répertoire: %s\n", path);
                    }
                }
                else
                {
                    if (change_directory(&fs, args[1]) == 0)
                    {
                        printf("Déplacement dans le répertoire\"%s\"\n", args[1]);
                    }
                    else
                    {
                        printf("Échec du déplacement dans le répertoire: %s\n", args[1]);
                    }
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
        else if (strcmp(args[0], "add_user") == 0)
        {
            if (args[1] == NULL || args[2] == NULL || args[3] == NULL)
            {
                printf("Usage : add_user <username> <group_id> <user_type>\n");
                printf("user_type : admin, user, guest\n");
            }
            else
            {
                uint32_t group_id = atoi(args[2]);
                UserType user_type;

                if (strcmp(args[3], "admin") == 0)
                {
                    user_type = USER_TYPE_ADMIN;
                }
                else if (strcmp(args[3], "user") == 0)
                {
                    user_type = USER_TYPE_USER;
                }
                else if (strcmp(args[3], "guest") == 0)
                {
                    user_type = USER_TYPE_GUEST;
                }
                else
                {
                    printf("Type d'utilisateur invalide. Utilisez : admin, user, guest\n");
                    continue;
                }

                uint32_t user_id = add_user(args[1], group_id, user_type);
                if (user_id == (uint32_t)-1)
                {
                    printf("Erreur : impossible d'ajouter l'utilisateur. Table pleine.\n");
                }
                else
                {
                    printf("Utilisateur ajouté avec succès : ID=%u, Nom=%s, Groupe=%u, Type=%s\n",
                           user_id, args[1], group_id, args[3]);
                }
            }
            continue;
        }
        else if (strcmp(args[0], "remove_user") == 0)
        {
            if (args[1] == NULL)
            {
                printf("Usage : rm_user <user_id>\n");
            }
            else
            {
                uint32_t user_id = atoi(args[1]);
                if (remove_user(user_id))
                {
                    printf("Utilisateur avec ID=%u supprimé avec succès.\n", user_id);
                }
                else
                {
                    printf("Erreur : impossible de supprimer l'utilisateur avec ID=%u.\n", user_id);
                }
            }
            continue;
        }
        else if (strcmp(args[0], "list_users") == 0)
        {
            display_users();
            continue;
        }
        else if (strcmp(args[0], "chmod") == 0)
        {
            if (args[1] == NULL || args[2] == NULL)
            {
                printf("Usage : chmod <path> <permissions>\n");
            }
            else
            {
                uint16_t permissions = strtol(args[2], NULL, 8); // Convertir les permissions en octal
                Inode *inode = get_inode_by_path(&fs, args[1]);  // Fonction pour récupérer l'inode par chemin
                if (inode == NULL)
                {
                    printf("Erreur : fichier ou répertoire introuvable : %s\n", args[1]);
                }
                else if (set_permissions(inode, permissions))
                {
                    printf("Permissions modifiées avec succès pour %s.\n", args[1]);
                }
                else
                {
                    printf("Erreur : impossible de modifier les permissions pour %s.\n", args[1]);
                }
            }
            continue;
        }
        else if (strcmp(args[0], "check_permissions") == 0)
        {
            if (args[1] == NULL || args[2] == NULL)
            {
                printf("Usage : check_permissions <path> <required_permissions>\n");
            }
            else
            {
                uint16_t required_permissions = strtol(args[2], NULL, 8); // Convertir les permissions en octal
                Inode *inode = get_inode_by_path(&fs, args[1]);
                if (inode == NULL)
                {
                    printf("Erreur : fichier ou répertoire introuvable : %s\n", args[1]);
                }
                else if (check_permissions(inode, required_permissions, get_current_user()))
                {
                    printf("L'utilisateur courant a les permissions nécessaires pour %s.\n", args[1]);
                }
                else
                {
                    printf("L'utilisateur courant n'a pas les permissions nécessaires pour %s.\n", args[1]);
                }
            }
            continue;
        }
        else if (strcmp(args[0], "chown") == 0)
        {
            if (args[1] == NULL || args[2] == NULL || args[3] == NULL)
            {
                printf("Usage : chown <path> <new_owner_id> <new_group_id>\n");
            }
            else
            {
                uint32_t new_owner_id = atoi(args[2]);
                uint32_t new_group_id = atoi(args[3]);
                Inode *inode = get_inode_by_path(&fs, args[1]); // Fonction pour récupérer l'inode par chemin
                if (inode == NULL)
                {
                    printf("Erreur : fichier ou répertoire introuvable : %s\n", args[1]);
                }
                else if (chown_inode(inode, new_owner_id, new_group_id))
                {
                    printf("Propriétaire et groupe modifiés avec succès pour %s.\n", args[1]);
                }
                else
                {
                    printf("Erreur : impossible de modifier le propriétaire et le groupe pour %s.\n", args[1]);
                }
            }
            continue;
        }
        else if (strcmp(args[0], "switch_user") == 0)
        {
            if (args[1] == NULL)
            {
                printf("Usage : switch_user <user_id>\n");
            }
            else
            {
                uint32_t user_id = atoi(args[1]);
                if (set_current_user(user_id))
                {
                    printf("Utilisateur courant changé avec succès : %s\n", get_current_user()->username);
                }
                else
                {
                    printf("Erreur : impossible de changer l'utilisateur courant à ID=%u.\n", user_id);
                }
            }
            continue;
        }
        else if (strcmp(args[0], "link") == 0)
        {
            if (args[1] == NULL)
            {
                printf("Il faut indiquer un fichier source\n");
            }
            if (args[2] == NULL)
            {
                printf("Il faut indiquer un chemin à créer\n");
            }
            if ((args[1] != NULL) && (args[2] != NULL))
            {
                /*
                char old_path[MAX_PATH_LEN];
                char new_path[MAX_PATH_LEN];
                const char *current_dir = get_current_directory(&fs);
                if ((args[1][0] != "/")&&(args[2][0] != "/"))
                {
                    if (current_dir[0] == '/')
                    {
                        snprintf(old_path, sizeof(old_path), "%s%s", current_dir, args[1]); // Garde le chemin tel quel (déjà absolu)
                        snprintf(new_path, sizeof(new_path), "%s%s", current_dir, args[1]);
                    }
                    else
                    {
                        snprintf(old_path, sizeof(old_path), "/%s%s", current_dir, args[1]); // Ajoute un "/" seulement si nécessaire
                        snprintf(old_path, sizeof(old_path), "/%s/%s", current_dir, args[1]);
                    }
                }
                else if ((args[1][0] == "/")&&(args[2][0] != "/"))
                {
                    if (current_dir[0] == '/')
                    {
                        snprintf(old_path, sizeof(old_path), "%s", args[1]); // Garde le chemin tel quel (déjà absolu)
                        snprintf(new_path, sizeof(new_path), "%s%s", current_dir, args[1]);
                    }
                    else
                    {
                        snprintf(old_path, sizeof(old_path), "%s", args[1]);
                        snprintf(old_path, sizeof(old_path), "/%s%s", current_dir, args[1]);// Ajoute un "/" seulement si nécessaire
                    }
                }*/
                if (fs_link(&fs, args[1], args[2]) == 0)
                {
                    printf("Fichiers liée avec succées: %s -> %s\n", args[1], args[2]);
                }
                else
                {
                    perror("Echecs lors de l'opération\n");
                }
            }
            continue;
        }
        else if (strcmp(args[0], "symlink") == 0)
        {
            if (args[1] == NULL)
            {
                printf("Il faut indiquer un fichier source\n");
            }
            if (args[2] == NULL)
            {
                printf("Il faut indiquer un chemin à créer\n");
            }
            if ((args[1] != NULL) && (args[2] != NULL))
            {
                if (fs_symlink(&fs, args[1], args[2]) == 0)
                {
                    printf("Fichiers liée symboliquement avec succées: %s -> %s\n", args[1], args[2]);
                }
                else
                {
                    perror("Échec de la création du lien symbolique");
                }
            }
            continue;
        }
        else if (strcmp(args[0], "read_symlink") == 0)
        {
            if (args[1] == NULL)
            {
                printf("Il faut indiquer un lien à lire\n");
            }
            if (args[1] != NULL)
            {
                char resolved_path[MAX_PATH_LEN];
                if (fs_readlink(&fs,find_inode_by_path(&fs,args[1]),resolved_path, MAX_PATH_LEN) == 0)
                {
                    printf("Le lien pointe vers: %s\n", resolved_path);
                }
            }
            continue;
        }
        else if (strcmp(args[0], "clear") == 0)
        {
            system("clear");
            continue;
        }
        else if (strcmp(args[0], "exit") == 0)
        {
            printf("Fermeture du shell. Au revoir !\n");
            exit(EXIT_SUCCESS);
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

int make_demo_directory(FileSystem *fs)
{

    if (!fs)
        return -1;
    // Création des répertoires avec permissions 0755
    if (create_directory(fs, "/home", 0755) == (uint32_t)-1)
        return -1;
    if (create_directory(fs, "/home/user", 0755) == (uint32_t)-1)
        return -1;
    if (create_directory(fs, "/home/guest", 0755) == (uint32_t)-1)
        return -1;
    if (create_directory(fs, "/home/user/documents", 0755) == (uint32_t)-1)
        return -1;
    if (create_directory(fs, "/home/user/photos", 0755) == (uint32_t)-1)
        return -1;
    if (create_directory(fs, "/bin", 0755) == (uint32_t)-1)
        return -1;
    if (create_directory(fs, "/etc", 0755) == (uint32_t)-1)
        return -1;

    // Création des fichiers exécutables avec permissions 0755
    if (create_file(fs, "/bin/ls", 0755) == (uint32_t)-1)
        return -1;
    if (create_file(fs, "/bin/sh", 0755) == (uint32_t)-1)
        return -1;

    // Création des fichiers de configuration avec permissions 0644
    if (create_file(fs, "/etc/config", 0644) == (uint32_t)-1)
        return -1;

    // Création du fichier passwd avec permissions restreintes 0600
    if (create_file(fs, "/etc/passwd", 0600) == (uint32_t)-1)
        return -1;

    return 0;
}

/**
 * @brief Initialise un utilisateur standard au démarrage.
 */
void initialize_default_user()
{
    // Ajouter un utilisateur standard (si la table est vide)
    if (get_user_count() == 0)
    {
        uint32_t default_user_id = add_user("default_user", 1, USER_TYPE_USER);
        if (default_user_id == (uint32_t)-1)
        {
            fprintf(stderr, "Erreur : impossible de créer l'utilisateur par défaut.\n");
            return;
        }
    }

    // Définir l'utilisateur standard comme utilisateur courant
    if (!set_current_user(0))
    {
        fprintf(stderr, "Erreur : impossible de définir l'utilisateur par défaut comme utilisateur courant.\n");
    }
    else
    {
        printf("Utilisateur par défaut connecté : %s\n", get_current_user()->username);
    }
}