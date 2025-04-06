#include <links.h>

/**
 * @brief Crée un lien physique (hard link) vers un fichier existant
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param oldpath Chemin existant du fichier
 * @param newpath Nouveau chemin à créer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int fs_link(FileSystem *fs, const char *oldpath, const char *newpath) {
    // 1. Vérifications de base
    if (!fs || !oldpath || !newpath) {
        fprintf(stderr, "Paramètres invalides\n");
        return -1;
    }

    // 2. Trouver l'inode du fichier existant
    uint32_t target_inode = find_inode_by_path(fs, oldpath);
    if (target_inode == (uint32_t)-1) {
        fprintf(stderr, "Fichier source non trouvé: %s\n", oldpath);
        return -1;
    }

    // 3. Vérifier que ce n'est pas un répertoire
    Inode *inode = &fs->inode_table[target_inode];
    if (inode->is_directory) {
        fprintf(stderr, "Impossible de créer un lien sur un répertoire\n");
        return -1;
    }

    // 4. Extraire le répertoire parent et le nom du nouveau lien
    char parent_path[MAX_PATH_LEN];
    char newname[MAX_FILENAME_LEN];
    if (!split_path(newpath, parent_path, newname)) {
        fprintf(stderr, "Chemin invalide: %s\n", newpath);
        return -1;
    }

    // 5. Vérifier que le parent existe et est un répertoire
    uint32_t parent_inode = find_inode_by_path(fs, parent_path);
    if (parent_inode == (uint32_t)-1) {
        fprintf(stderr, "Répertoire parent non trouvé: %s\n", parent_path);
        return -1;
    }

    Inode *parent = &fs->inode_table[parent_inode];
    if (!parent->is_directory) {
        fprintf(stderr, "Le parent n'est pas un répertoire: %s\n", parent_path);
        return -1;
    }

    // 6. Vérifier que le nouveau nom n'existe pas déjà
    if (find_file_in_directory(fs, parent_inode, newname) != (uint32_t)-1) {
        fprintf(stderr, "Le nom existe déjà: %s\n", newname);
        return -1;
    }

    // 7. Ajouter l'entrée dans le répertoire parent
    if (!add_directory_entry(fs, parent_inode, target_inode, newname)) {
        fprintf(stderr, "Échec de l'ajout au répertoire\n");
        return -1;
    }

    // 8. Incrémenter le compteur de liens
    inode->links_count++;
    inode->modified_at = (uint64_t)time(NULL);

    return 0;
}

/**
 * @brief Crée un lien symbolique (soft link)
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param target Chemin cible du lien
 * @param linkpath Chemin du lien à créer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int fs_symlink(FileSystem *fs, const char *target, const char *linkpath) {
    // 1. Vérifications de base
    if (!fs || !target || !linkpath) {
        fprintf(stderr, "Paramètres invalides\n");
        return -1;
    }

    // 2. Extraire le répertoire parent et le nom du lien
    char parent_path[MAX_PATH_LEN];
    char linkname[MAX_FILENAME_LEN];
    if (!split_path(linkpath, parent_path, linkname)) {
        fprintf(stderr, "Chemin invalide: %s\n", linkpath);
        return -1;
    }

    // 3. Vérifier que le parent existe et est un répertoire
    uint32_t parent_inode = find_inode_by_path(fs, parent_path);
    if (parent_inode == (uint32_t)-1) {
        fprintf(stderr, "Répertoire parent non trouvé: %s\n", parent_path);
        return -1;
    }

    Inode *parent = &fs->inode_table[parent_inode];
    if (parent->type != FILE_DIRECTORY) {
        fprintf(stderr, "Le parent n'est pas un répertoire: %s\n", parent_path);
        return -1;
    }

    // 4. Vérifier que le lien n'existe pas déjà
    if (find_file_in_directory(fs, parent_inode, linkname) != (uint32_t)-1) {
        fprintf(stderr, "Le nom existe déjà: %s\n", linkname);
        return -1;
    }

    // 5. Créer un nouvel inode pour le lien symbolique
    uint32_t new_inode = allocate_inode(fs);
    if (new_inode == (uint32_t)-1) {
        fprintf(stderr, "Plus d'inodes disponibles\n");
        return -1;
    }

    Inode *inode = &fs->inode_table[new_inode];
    inode->type = FILE_SYMLINK;
    inode->links_count = 1;
    inode->size = strlen(target);
    inode->permissions = 0777; // lrwxrwxrwx

    // 6. Allouer et copier la cible du lien
    inode->symlink_target = malloc(strlen(target) + 1);
    if (!inode->symlink_target) {
        free_inode(fs, new_inode);
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return -1;
    }
    strcpy(inode->symlink_target, target);

    // 7. Ajouter l'entrée dans le répertoire parent
    if (!add_directory_entry(fs, parent_inode, new_inode, linkname)) {
        free(inode->symlink_target);
        free_inode(fs, new_inode);
        fprintf(stderr, "Échec de l'ajout au répertoire\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Résout un lien symbolique
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin contenant potentiellement des liens symboliques
 * @param resolved_path Buffer pour stocker le chemin résolu
 * @param size Taille du buffer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int fs_readlink(FileSystem *fs, const char *path, char *resolved_path, size_t size) {
    // 1. Trouver l'inode du lien
    uint32_t inode_id = find_inode_by_path(fs, path);
    if (inode_id == (uint32_t)-1) {
        fprintf(stderr, "Lien non trouvé: %s\n", path);
        return -1;
    }

    Inode *inode = &fs->inode_table[inode_id];
    if (inode->type != FILE_SYMLINK) {
        fprintf(stderr, "N'est pas un lien symbolique: %s\n", path);
        return -1;
    }

    // 2. Vérifier la taille du buffer
    if (strlen(inode->symlink_target) >= size) {
        fprintf(stderr, "Buffer trop petit pour le lien\n");
        return -1;
    }

    // 3. Copier la cible dans le buffer
    strcpy(resolved_path, inode->symlink_target);
    return 0;
}