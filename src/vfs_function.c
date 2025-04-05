#include <vfs_function.h>
/**
 * @brief Liste le contenu d'un répertoire (équivalent à ls)
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin du répertoire à lister
 * @param long_format Si true, affiche les détails complets (comme ls -l)
 * @return int 0 en cas de succès, -1 en cas d'erreur
 */
int list_directory(FileSystem *fs, const char *path, bool long_format) {
    // 1. Trouver l'inode du répertoire
    uint32_t dir_inode = find_inode_by_path(fs, path);
    if (dir_inode == (uint32_t)-1) {
        fprintf(stderr, "Erreur: répertoire '%s' non trouvé\n", path);
        return -1;
    }

    // 2. Vérifier que c'est bien un répertoire
    Inode *inode = &fs->inode_table[dir_inode];
    if (!inode->is_directory) {
        fprintf(stderr, "Erreur: '%s' n'est pas un répertoire\n", path);
        return -1;
    }

    // 3. Lire le contenu du répertoire
    Directory dir;
    if (!read_directory(fs, dir_inode, &dir)) {
        fprintf(stderr, "Erreur: impossible de lire le répertoire\n");
        return -1;
    }

    // 4. Afficher le contenu
    if (long_format) {
        printf("total %u\n", dir.entry_count);
        for (uint32_t i = 0; i < dir.entry_count; i++) {
            Inode *entry_inode = &fs->inode_table[dir.entries[i]];
            
            // Permissions (comme ls -l)
            printf("%c", entry_inode->is_directory ? 'd' : '-');
            printf("%c", (entry_inode->permissions & 0400) ? 'r' : '-');
            printf("%c", (entry_inode->permissions & 0200) ? 'w' : '-');
            printf("%c", (entry_inode->permissions & 0100) ? 'x' : '-');
            printf("%c", (entry_inode->permissions & 0040) ? 'r' : '-');
            printf("%c", (entry_inode->permissions & 0020) ? 'w' : '-');
            printf("%c", (entry_inode->permissions & 0010) ? 'x' : '-');
            printf("%c", (entry_inode->permissions & 0004) ? 'r' : '-');
            printf("%c", (entry_inode->permissions & 0002) ? 'w' : '-');
            printf("%c ", (entry_inode->permissions & 0001) ? 'x' : '-');
            
            // Nombre de liens
            printf("%2u ", entry_inode->links_count);
            
            // Propriétaire et groupe (simplifié)
            printf("%4u %4u ", entry_inode->owner_id, entry_inode->group_id);
            
            // Taille
            printf("%8u ", entry_inode->size);
            
            // Date de modification
            char date_buf[20];
            strftime(date_buf, sizeof(date_buf), "%b %d %H:%M", localtime((time_t*)&entry_inode->modified_at));
            printf("%s ", date_buf);
            
            // Nom du fichier
            printf("%s\n", dir.names[i]);
        }
    } else {
        // Format simple (comme ls)
        for (uint32_t i = 0; i < dir.entry_count; i++) {
            printf("%s  ", dir.names[i]);
        }
        printf("\n");
    }

    return 0;
}

/**
 * @brief Change le répertoire courant
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin du nouveau répertoire courant
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int change_directory(FileSystem *fs, const char *path) {
    if (!fs || !path) {
        fprintf(stderr, "Paramètres invalides\n");
        return -1;
    }

    // Trouver l'inode cible
    uint32_t target_inode = find_inode_by_path(fs, path);
    if (target_inode == (uint32_t)-1 || target_inode >= MAX_FILES) {
        fprintf(stderr, "Répertoire non trouvé: %s\n", path);
        return -1;
    }

    // Vérifier que c'est un répertoire
    Inode *inode = &fs->inode_table[target_inode];
    if (!inode->is_directory) {
        fprintf(stderr, "N'est pas un répertoire: %s\n", path);
        return -1;
    }

    // Mettre à jour le répertoire courant
    fs->current_directory = target_inode;

    // Mettre à jour le chemin courant
    if (path[0] == '/') {
        // Chemin absolu - utiliser directement
        strncpy(fs->current_path, path, MAX_PATH_LEN);
    } else {
        // Chemin relatif - construire le nouveau chemin
        if (strcmp(path, "..") == 0) {
            // Cas spécial: remonter d'un niveau
            char *last_slash = strrchr(fs->current_path, '/');
            if (last_slash) {
                if (last_slash == fs->current_path) {
                    // Cas racine
                    fs->current_path[1] = '\0';
                } else {
                    *last_slash = '\0';
                }
            }
        } else if (strcmp(path, ".") != 0) {
            // Ajouter le nouveau segment
            if (strcmp(fs->current_path, "/") != 0) {
                strncat(fs->current_path, "/", MAX_PATH_LEN - strlen(fs->current_path) - 1);
            }
            strncat(fs->current_path, path, MAX_PATH_LEN - strlen(fs->current_path) - 1);
        }
    }

    // Simplifier le chemin (supprimer les doublons de '/', etc.)
    simplify_path(fs->current_path);
    
    // Assurer que le chemin se termine par '\0'
    fs->current_path[MAX_PATH_LEN - 1] = '\0';

    return 0;
}

/**
 * @brief Obtient le chemin absolu du répertoire courant
 * 
 * @param fs Pointeur vers le système de fichiers
 * @return const char* Chemin absolu du répertoire courant
 */
const char* get_current_directory(FileSystem *fs) {
    if (!fs) return NULL;
    return fs->current_path;
}

/**
 * @brief Convertit un chemin relatif en chemin absolu
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param rel_path Chemin relatif
 * @param abs_path Buffer pour le chemin absolu
 * @param size Taille du buffer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int resolve_relative_path(FileSystem *fs, const char *rel_path, char *abs_path, size_t size) {
    if (!fs || !rel_path || !abs_path || size < 2) {
        return -1;
    }

    if (rel_path[0] == '/') {
        // C'est déjà un chemin absolu
        strncpy(abs_path, rel_path, size);
        return 0;
    }

    // Construire le chemin absolu
    if (snprintf(abs_path, size, "%s/%s", fs->current_path, rel_path) >= (int)size) {
        return -1; // Buffer trop petit
    }

    // Simplifier le chemin (enlever les ./ et ../ inutiles)
    simplify_path(abs_path);
    
    return 0;
}

/**
 * @brief Simplifie un chemin en supprimant les . et .. inutiles
 * 
 * @param path Chemin à simplifier (modifié sur place)
 */
void simplify_path(char *path) {
    char *p = path;
    char *out = path;
    int after_sep = 1; // Éviter les doublons de '/'

    while (*p) {
        if (*p == '/') {
            if (!after_sep) {
                *out++ = '/';
                after_sep = 1;
            }
            p++;
        } else {
            *out++ = *p++;
            after_sep = 0;
        }
    }
    
    // Supprimer le slash final sauf pour la racine
    if (out > path + 1 && *(out-1) == '/') {
        out--;
    }
    
    *out = '\0';
}