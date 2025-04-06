/**
 * @file tree.h
 * @brief Affichage de l'arborescence du système de fichiers
 *
 * @details Ce fichier d'en-tête définit les fonctions permettant d'afficher
 *          la structure hiérarchique du système de fichiers sous forme d'arbre,
 *          avec prise en charge de la profondeur maximale et de l'indentation.
 *
 * @version 1.0
 * @date 06/04/2025
 *
 * @authors
 * - Robin de Angelis 0 (%)
 * - Alexandre Ledard 100(%)
 * - Killian Treuil 0(%)
 *
 * @defgroup tree_visualisation Visualisation arborescente
 * @{
 */

 #ifndef TREE_H
 #define TREE_H
 
 #include <partition.h>
 #include <vfs_function.h>
 
 /**
  * @brief Affiche l'arborescence du système de fichiers
  * 
  * @param fs Pointeur vers le système de fichiers
  * @param path Chemin de départ (NULL pour la racine)
  * @param max_depth Profondeur maximale d'affichage
  */
 void tree(FileSystem *fs, const char *path, int max_depth);
 
 /**
  * @brief Fonction récursive interne pour l'affichage arborescent
  * 
  * @param fs Pointeur vers le système de fichiers
  * @param dir_inode Inode du répertoire courant
  * @param current_depth Profondeur actuelle de récursion
  * @param max_depth Profondeur maximale autorisée
  * @param prefix Préfixe d'affichage pour l'indentation
  */
 static void print_tree_recursive(FileSystem *fs, uint32_t dir_inode,
     int current_depth, int max_depth,
     const char *prefix);
 
 #endif // TREE_H
 
 /** @} */ // Fin du groupe tree_visualisation