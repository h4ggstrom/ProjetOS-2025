#ifndef VFS_FUNCTION_H
#define VFS_FUNCTION_HANTES_H
#include <constantes.h>
#include <partition.h>
#include <sys/types.h>
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

/**
 * @brief Écrit des données dans un fichier
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param fd Descripteur de fichier
 * @param buf Données à écrire
 * @param count Nombre d'octets à écrire
 * @return ssize_t Nombre d'octets écrits, ou -1 en cas d'erreur
 */
ssize_t fs_write(FileSystem *fs, int fd, const void *buf, size_t count);

/**
 * @brief Lit des données depuis un fichier
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param fd Descripteur de fichier
 * @param buf Buffer de destination
 * @param count Nombre d'octets à lire
 * @return ssize_t Nombre d'octets lus, ou -1 en cas d'erreur
 */
ssize_t fs_read(FileSystem *fs, int fd, void *buf, size_t count);

/**
 * @brief Étend un fichier avec des zéros si nécessaire
 */
static int extend_file(FileSystem *fs, Inode *inode, uint32_t bytes_needed);

/**
 * @brief Obtient le bloc physique correspondant à un bloc logique
 */
uint32_t get_physical_block(FileSystem *fs, Inode *inode, uint32_t logical_block) ;

/**
 * @brief Repositionne l'offset de lecture/écriture d'un fichier
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param fd Descripteur de fichier
 * @param offset Décalage selon le paramètre whence
 * @param whence Origine de positionnement (SEEK_SET, SEEK_CUR, SEEK_END)
 * @return off_t Nouvelle position, ou -1 en cas d'erreur
 */
off_t fs_lseek(FileSystem *fs, int fd, off_t offset, int whence) ;

/**
 * @brief Vérifie si un chemin est relatif
 * @param path Le chemin à vérifier
 * @return true si le chemin est relatif, false sinon
 */
bool is_relative_path(const char *path);
#endif