#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <stdint.h>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/**********************************************
 *          CONSTANTES DU SYSTÈME             *
 **********************************************/

#define MAGIC_NUMBER "FS2025"    // Signature magique du système de fichiers
#define VERSION "0.3"           // Version du système de fichiers

/**********************************************
 *         LIMITES DU SYSTÈME                 *
 **********************************************/

#define MAX_FILES 1024          // Nombre maximal de fichiers (inodes)
#define MAX_OPEN_FILES 64       // Nombre maximal de fichiers ouverts simultanément
#define MAX_FILENAME_LEN 255    // Longueur maximale d'un nom de fichier
#define MAX_PATH_LEN 1024       // Longueur maximale d'un chemin
#define MAX_BLOCKS_PTR 12       // Nombre de pointeurs directs dans un inode
#define DIR_ENTRIES_LIMIT 128   // Nombre maximal d'entrées dans un répertoire
#define MAX_PATH_DEPTH 32   // Profondeur maximale des répertoires

/**********************************************
 *          TAILLES DES STRUCTURES            *
 **********************************************/

#define BLOCK_SIZE_DEFAULT 16384  // Taille par défaut d'un bloc
#define SUPERBLOCK_SIZE 512      // Taille du superbloc
#define INODE_SIZE 128           // Taille d'un inode
#define DIR_ENTRY_SIZE 32        // Taille d'une entrée de répertoire

/**********************************************
 *         MODES D'OUVERTURE                  *
 * (inspirés de fcntl.h)                      *
 **********************************************/

#define O_RDONLY    0x0001      // Lecture seule
#define O_WRONLY    0x0002      // Écriture seule
#define O_RDWR      0x0004      // Lecture et écriture
#define O_CREAT     0x0100      // Créer le fichier s'il n'existe pas
#define O_TRUNC     0x0200      // Tronquer le fichier existant
#define O_APPEND    0x0400      // Écrire à la fin du fichier
#define O_EXCL      0x0800      // Échec si le fichier existe (avec O_CREAT)

/**********************************************
 *         PERMISSIONS UNIX                   *
 **********************************************/

#define S_IRUSR 0400    // Read by owner
#define S_IWUSR 0200    // Write by owner
#define S_IXUSR 0100    // Execute by owner
#define S_IRGRP 0040    // Read by group
#define S_IWGRP 0020    // Write by group
#define S_IXGRP 0010    // Execute by group
#define S_IROTH 0004    // Read by others
#define S_IWOTH 0002    // Write by others
#define S_IXOTH 0001    // Execute by others

#define S_IFREG 0100000 // Regular file
#define S_IFDIR 0040000 // Directory
#define S_IFMT  0170000 // Masque pour le type de fichier

/**********************************************
 *         CODES D'ERREUR                     *
 **********************************************/

#define SUCCESS     0  // Opération réussie
#define E_NOTFOUND   -1  // Fichier non trouvé
#define E_EXISTS     -2  // Fichier existe déjà
#define E_PERM       -3  // Permission refusée
#define E_IO         -4  // Erreur d'E/S
#define E_NOMEM      -5  // Mémoire insuffisante
#define E_INVAL      -6  // Paramètre invalide
#define E_MAXFILES   -7  // Trop de fichiers ouverts
#define E_ISDIR      -8  // Est un répertoire
#define E_NOTDIR     -9  // N'est pas un répertoire
#define E_NOSPC      -10 // Plus d'espace disponible
#define E_FD         -11 // Descripteur de fichier invalide (négatif)

/**********************************************
 *         OFFSETS DANS LE DISQUE             *
 **********************************************/

#define SUPERBLOCK_OFFSET 0
#define INODE_TABLE_OFFSET SUPERBLOCK_SIZE
#define BLOCK_BITMAP_OFFSET (INODE_TABLE_OFFSET + (MAX_FILES * INODE_SIZE))
#define DATA_BLOCKS_OFFSET (BLOCK_BITMAP_OFFSET + (MAX_FILES / 8))

#endif // CONSTANTES_H