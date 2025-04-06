#ifndef LINKS_H
#define LINKS_H

#include <partition.h>

/**
 * @brief Crée un lien physique (hard link) vers un fichier existant
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param oldpath Chemin existant du fichier
 * @param newpath Nouveau chemin à créer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int fs_link(FileSystem *fs, const char *oldpath, const char *newpath);

/**
 * Create a hard link between two files.
 * 
 * @param target The target file to link to.
 * @param linkpath The path of the new hard link.
 * @return 0 on success, -1 on failure.
 */
int create_hard_link(const char *target, const char *linkpath);

/**
 * Create a symbolic (soft) link between two files.
 * 
 * @param target The target file to link to.
 * @param linkpath The path of the new symbolic link.
 * @return 0 on success, -1 on failure.
 */
int create_soft_link(const char *target, const char *linkpath);

/**
 * Display the target of a symbolic link.
 * 
 * @param linkpath The path of the symbolic link.
 * @return 0 on success, -1 on failure.
 */
int display_soft_link_target(const char *linkpath);

#endif // LINKS_H