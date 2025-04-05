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
#endif