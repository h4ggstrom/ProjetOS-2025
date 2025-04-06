/**
 * @file vfs_function.c
 * @brief Partie du programme qui regroupe les commandes du terminales pour intéragir avec le File System.
 *
 *
 * @version 0.3
 * @date 04/04/2025
 *
 * @authors
 * - Robin de Angelis (%)
 * - Alexandre Ledard (%)
 * - Killian Treuil (%)
 */

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
 * @param fs Le système de fichiers
 * @param relative_path Le chemin relatif
 * @param absolute_path Buffer pour stocker le chemin absolu
 * @param buffer_size Taille du buffer
 * @return 0 en cas de succès, -1 en cas d'erreur
 */
int resolve_relative_path(FileSystem *fs, const char *relative_path, char *absolute_path, size_t buffer_size) {
    const char *current_dir = get_current_directory(fs);
    
    if(strcmp(current_dir, "/") == 0){
    // Éviter les doublons de '/'
    if (current_dir[strlen(current_dir) - 1] == '/' && relative_path[0] == '/') {
        snprintf(absolute_path, buffer_size, "%s%s", current_dir, relative_path + 1);
    } else if (current_dir[strlen(current_dir) - 1] != '/' && relative_path[0] != '/') {
        snprintf(absolute_path, buffer_size, "%s/%s", current_dir, relative_path);
    } else {
        snprintf(absolute_path, buffer_size, "%s%s", current_dir, relative_path);
    }
}
else{
    // Éviter les doublons de '/'
    if (current_dir[strlen(current_dir) - 1] == '/' && relative_path[0] == '/') {
        snprintf(absolute_path, buffer_size, "%s%s", current_dir, relative_path + 1);
    } else if (current_dir[strlen(current_dir) - 1] != '/' && relative_path[0] != '/') {
        snprintf(absolute_path, buffer_size, "/%s/%s", current_dir, relative_path);
    } else {
        snprintf(absolute_path, buffer_size, "%s%s", current_dir, relative_path);
}
}
    
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

/**
 * @brief Écrit des données dans un fichier
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param fd Descripteur de fichier
 * @param buf Données à écrire
 * @param count Nombre d'octets à écrire
 * @return ssize_t Nombre d'octets écrits, ou -1 en cas d'erreur
 */
ssize_t fs_write(FileSystem *fs, int fd, const void *buf, size_t count) {
    // 1. Vérifications de base
    if (!fs || fd < 0 || fd >= fs->max_open_files || !fs->open_files_table[fd].is_used) {
        fprintf(stderr, "Descripteur de fichier invalide\n");
        return -1;
    }

    if (!buf && count > 0) {
        fprintf(stderr, "Buffer invalide\n");
        return -1;
    }

    // 2. Vérifier les permissions
    FileDescriptor *fdesc = &fs->open_files_table[fd];
    if (!(fdesc->mode & O_WRONLY) && !(fdesc->mode & O_RDWR)) {
        fprintf(stderr, "Permission refusée (fichier non ouvert en écriture)\n");
        return -1;
    }

    // 3. Obtenir l'inode et vérifier les permissions
    Inode *inode = &fs->inode_table[fdesc->inode_id];
    if (!(inode->permissions & 0222)) {
        fprintf(stderr, "Permissions d'écriture refusées\n");
        return -1;
    }

    // 4. Calculer la position maximale
    uint32_t max_position = inode->size;
    if (fdesc->current_pos > max_position) {
        // Si on écrit après la fin du fichier, étendre avec des zéros
        if (extend_file(fs, inode, fdesc->current_pos - max_position) < 0) {
            return -1;
        }
        max_position = fdesc->current_pos;
    }

    // 5. Écrire les données bloc par bloc
    size_t bytes_written = 0;
    const uint8_t *data = (const uint8_t *)buf;

    while (bytes_written < count) {
        // Calculer le bloc logique et l'offset dans le bloc
        uint32_t logical_block = fdesc->current_pos / fs->superblock.block_size;
        uint32_t block_offset = fdesc->current_pos % fs->superblock.block_size;
        uint32_t bytes_to_write = MIN(fs->superblock.block_size - block_offset, 
                                     count - bytes_written);

        // Allouer ou trouver le bloc physique
        uint32_t physical_block = allocate_block_for_inode(fs, inode, logical_block);
        if (physical_block == (uint32_t)-1) {
            fprintf(stderr, "Erreur d'allocation de bloc\n");
            break;
        }

        // Écrire les données dans le bloc
        memcpy(fs->partition.blocks[physical_block].data + block_offset,
               data + bytes_written,
               bytes_to_write);

        // Mettre à jour les positions
        bytes_written += bytes_to_write;
        fdesc->current_pos += bytes_to_write;

        // Mettre à jour la taille du fichier si nécessaire
        if (fdesc->current_pos > inode->size) {
            inode->size = fdesc->current_pos;
        }
    }

    // 6. Mettre à jour les métadonnées
    inode->modified_at = (uint64_t)time(NULL);

    return (ssize_t)bytes_written;
}

/**
 * @brief Étend un fichier avec des zéros si nécessaire
 */
static int extend_file(FileSystem *fs, Inode *inode, uint32_t bytes_needed) {
    uint32_t blocks_needed = (bytes_needed + fs->superblock.block_size - 1) / 
                            fs->superblock.block_size;

    for (uint32_t i = 0; i < blocks_needed; i++) {
        uint32_t logical_block = (inode->size + i * fs->superblock.block_size) / 
                                fs->superblock.block_size;
        if (allocate_block_for_inode(fs, inode, logical_block) == (uint32_t)-1) {
            return -1;
        }
    }
    return 0;
}

 /**
 * @brief Lit des données depuis un fichier
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param fd Descripteur de fichier
 * @param buf Buffer de destination
 * @param count Nombre d'octets à lire
 * @return ssize_t Nombre d'octets lus, ou -1 en cas d'erreur
 */
ssize_t fs_read(FileSystem *fs, int fd, void *buf, size_t count) {
    // 1. Vérifications de base
    if (!fs || fd < 0 || fd >= fs->max_open_files || !fs->open_files_table[fd].is_used) {
        fprintf(stderr, "Descripteur de fichier invalide\n");
        return -1;
    }

    if (!buf && count > 0) {
        fprintf(stderr, "Buffer invalide\n");
        return -1;
    }

    // 2. Vérifier les permissions
    FileDescriptor *fdesc = &fs->open_files_table[fd];
    if (!(fdesc->mode & O_RDONLY) && !(fdesc->mode & O_RDWR)) {
        fprintf(stderr, "Permission refusée (fichier non ouvert en lecture)\n");
        return -1;
    }

    // 3. Obtenir l'inode et vérifier les permissions
    Inode *inode = &fs->inode_table[fdesc->inode_id];
    if (!(inode->permissions & 0444)) {
        fprintf(stderr, "Permissions de lecture refusées\n");
        return -1;
    }

    // 4. Calculer la quantité maximale à lire
    size_t max_readable = inode->size - fdesc->current_pos;
    if (count > max_readable) {
        count = max_readable;
    }
    if (count == 0) {
        return 0; // EOF
    }

    // 5. Lire les données bloc par bloc
    size_t bytes_read = 0;
    uint8_t *data = (uint8_t *)buf;

    while (bytes_read < count) {
        // Calculer le bloc logique et l'offset dans le bloc
        uint32_t logical_block = fdesc->current_pos / fs->superblock.block_size;
        uint32_t block_offset = fdesc->current_pos % fs->superblock.block_size;
        uint32_t bytes_to_read = MIN(fs->superblock.block_size - block_offset, 
                                   count - bytes_read);

        // Trouver le bloc physique
        uint32_t physical_block = get_physical_block(fs, inode, logical_block);
        if (physical_block == (uint32_t)-1) {
            // Bloc non alloué = considéré comme rempli de zéros
            memset(data + bytes_read, 0, bytes_to_read);
        } else {
            // Copier les données depuis le bloc
            memcpy(data + bytes_read,
                   fs->partition.blocks[physical_block].data + block_offset,
                   bytes_to_read);
        }

        // Mettre à jour les positions
        bytes_read += bytes_to_read;
        fdesc->current_pos += bytes_to_read;
    }
    // 6. Mettre à jour le timestamp d'accès
    inode->accessed_at = (uint64_t)time(NULL);

    return (ssize_t)bytes_read;
}

/**
 * @brief Obtient le bloc physique correspondant à un bloc logique
*/
uint32_t get_physical_block(FileSystem *fs, Inode *inode, uint32_t logical_block) {
    // Blocs directs (0-11)
    if (logical_block < 12) {
        return inode->blocks[logical_block];
    }

    // Bloc indirect simple (12-...)
    logical_block -= 12;
    if (inode->indirect_block == 0) {
        return (uint32_t)-1;
    }

    if (logical_block < MAX_BLOCKS_PTR) {
        uint32_t *block_pointers = (uint32_t *)fs->partition.blocks[inode->indirect_block].data;
        return block_pointers[logical_block];
    }

    return (uint32_t)-1;
}

/**
 * @brief Repositionne l'offset de lecture/écriture d'un fichier
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param fd Descripteur de fichier
 * @param offset Décalage selon le paramètre whence
 * @param whence Origine de positionnement (SEEK_SET, SEEK_CUR, SEEK_END)
 * @return off_t Nouvelle position, ou -1 en cas d'erreur
 */
off_t fs_lseek(FileSystem *fs, int fd, off_t offset, int whence) {
    // 1. Vérifications de base
    if (!fs || fd < 0 || fd >= fs->max_open_files || !fs->open_files_table[fd].is_used) {
        fprintf(stderr, "Descripteur de fichier invalide\n");
        return -1;
    }

    FileDescriptor *fdesc = &fs->open_files_table[fd];
    Inode *inode = &fs->inode_table[fdesc->inode_id];
    off_t new_pos;

    // 2. Calculer la nouvelle position selon 'whence'
    switch (whence) {
        case SEEK_SET: // Début du fichier
            new_pos = offset;
            break;

        case SEEK_CUR: // Position actuelle
            new_pos = fdesc->current_pos + offset;
            break;

        case SEEK_END: // Fin du fichier
            new_pos = inode->size + offset;
            break;

        default:
            fprintf(stderr, "Valeur 'whence' invalide\n");
            return -1;
    }

    // 3. Validation de la nouvelle position
    if (new_pos < 0) {
        fprintf(stderr, "Position invalide (négative)\n");
        return -1;
    }

    // 4. Optionnel: Autoriser le positionnement après la fin du fichier
    // (Comportement standard Unix)
    if (new_pos > inode->size) {
        // Si le fichier est ouvert en écriture, étendre avec des zéros
        if ((fdesc->mode & O_WRONLY) || (fdesc->mode & O_RDWR)) {
            uint32_t extension = new_pos - inode->size;
            if (extend_file(fs, inode, extension) < 0) {
                return -1;
            }
        }
    }

    // 5. Mettre à jour la position
    fdesc->current_pos = new_pos;

    return new_pos;
}

/**
 * @brief Vérifie si un chemin est relatif
 * @param path Le chemin à vérifier
 * @return true si le chemin est relatif, false sinon
 */
bool is_relative_path(const char *path) {
    if (path == NULL || strlen(path) == 0) {
        return false; // Cas invalide,
    }
    return (path[0] != '/');
}