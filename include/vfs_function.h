#ifndef VFS_FUNCTION_H
#define VFS_FUNCTION_HANTES_H
#include <constantes.h>
#include <partition.h>
/**
 * @brief Liste le contenu d'un répertoire (équivalent à ls)
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin du répertoire à lister
 * @param long_format Si true, affiche les détails complets (comme ls -l)
 * @return int 0 en cas de succès, -1 en cas d'erreur
 */
int list_directory(FileSystem *fs, const char *path, bool long_format);

/**
 * @brief Change le répertoire courant
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin du nouveau répertoire courant
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int change_directory(FileSystem *fs, const char *path);

/**
 * @brief Obtient le chemin absolu du répertoire courant
 * 
 * @param fs Pointeur vers le système de fichiers
 * @return const char* Chemin absolu du répertoire courant
 */
const char* get_current_directory(FileSystem *fs);

/**
 * @brief Convertit un chemin relatif en chemin absolu
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param rel_path Chemin relatif
 * @param abs_path Buffer pour le chemin absolu
 * @param size Taille du buffer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int resolve_relative_path(FileSystem *fs, const char *rel_path, char *abs_path, size_t size);

/**
 * @brief Simplifie un chemin en supprimant les . et .. inutiles
 * 
 * @param path Chemin à simplifier (modifié sur place)
 */
void simplify_path(char *path);

#endif