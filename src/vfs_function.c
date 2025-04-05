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