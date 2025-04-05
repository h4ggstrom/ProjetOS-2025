#include <tree.h>
/**
 * @brief Affiche l'arborescence du système de fichiers
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin de départ (si NULL, commence à la racine)
 * @param max_depth Profondeur maximale à afficher (0 pour illimité)
 */
void tree(FileSystem *fs, const char *path, int max_depth) {
    if (!fs) return;

    // Déterminer le répertoire de départ
    uint32_t start_inode;
    if (path) {
        start_inode = find_inode_by_path(fs, path);
        if (start_inode == (uint32_t)-1) {
            printf("Chemin non trouvé: %s\n", path);
            return;
        }
    } else {
        start_inode = 0; // Racine par défaut
    }

    // Vérifier que c'est un répertoire
    Inode *inode = &fs->inode_table[start_inode];
    if (!inode->is_directory) {
        printf("Le chemin spécifié n'est pas un répertoire\n");
        return;
    }

    // Afficher l'en-tête
    printf(".\n");
    
    // Appel récursif
    print_tree_recursive(fs, start_inode, 0, max_depth, "");
}

/**
 * @brief Fonction récursive pour afficher l'arborescence
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param dir_inode Inode du répertoire courant
 * @param current_depth Profondeur actuelle
 * @param max_depth Profondeur maximale
 * @param prefix Préfixe pour l'affichage
 */
static void print_tree_recursive(FileSystem *fs, uint32_t dir_inode, 
                               int current_depth, int max_depth, 
                               const char *prefix) {
    // Arrêter si on a atteint la profondeur maximale
    if (max_depth > 0 && current_depth >= max_depth) {
        return;
    }

    // Lire le répertoire
    Directory dir;
    if (!read_directory(fs, dir_inode, &dir)) {
        return;
    }

    // Parcourir toutes les entrées (sauf . et ..)
    for (uint32_t i = 0; i < dir.entry_count; i++) {
        if (strcmp(dir.names[i], ".") == 0 || strcmp(dir.names[i], "..") == 0) {
            continue;
        }

        // Afficher l'entrée courante
        printf("%s", prefix);
        
        // Dernier élément ou non?
        if (i == dir.entry_count - 1) {
            printf("└── ");
        } else {
            printf("├── ");
        }
        
        printf("%s", dir.names[i]);

        // Vérifier si c'est un répertoire
        Inode *entry_inode = &fs->inode_table[dir.entries[i]];
        if (entry_inode->is_directory) {
            printf("/\n");
            
            // Construire le nouveau préfixe
            char new_prefix[256];
            snprintf(new_prefix, sizeof(new_prefix), "%s%s", 
                    prefix, 
                    (i == dir.entry_count - 1) ? "    " : "│   ");
            
            // Appel récursif
            print_tree_recursive(fs, dir.entries[i], current_depth + 1, 
                                max_depth, new_prefix);
        } else {
            printf("\n");
        }
    }
}