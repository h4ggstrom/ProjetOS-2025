#ifndef TREE_H
#define TREE_H

#include <partition.h>
#include <vfs_function.h> 
/**
 * @brief Affiche l'arborescence du système de fichiers
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin de départ (si NULL, commence à la racine)
 * @param max_depth Prof
 * */
void tree(FileSystem *fs, const char *path, int max_depth);

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
    const char *prefix);
#endif // FILE_OPERATIONS_H