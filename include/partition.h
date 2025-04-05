#ifndef PARTITION_H
#define PARTITION_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <constantes.h>

/**
 * @file partition.h
 * @brief Header file for the partition management library.
 * 
 * @details This file declares the structures and functions used to manage
 *          a file system partition, including block management, inode handling,
 *          and directory operations.
 * 
 * @version 0.3
 * @date 04/04/2025
 * 
 * @authors
 * - Robin de Angelis (%)
 * - Alexandre Ledard (%)
 * - Killian Treuil (%)
 */

/**
 * @struct Block
 * @brief Represents a block in the partition.
 * 
 * @details A block is a fixed-size unit of storage in the partition. Each block
 *          can store data and is tracked using a bitmap.
 */
typedef struct Block {
    uint32_t index;       ///< Index of the block in the partition
    uint8_t *data;        ///< Pointer to the block's data
    bool is_free;         ///< Indicates whether the block is free or used
} Block;

/**
 * @struct Inode
 * @brief Represents an inode in the file system.
 * 
 * @details An inode contains metadata about a file or directory, such as its size,
 *          permissions, and pointers to the data blocks.
 */
typedef struct Inode {
    uint32_t id;               ///< Unique identifier for the inode
    uint32_t size;             ///< Size of the file (in bytes)
    uint32_t blocks[12];       ///< Direct pointers to data blocks
    uint32_t indirect_block;   ///< Pointer to an indirect block (if needed)
    uint32_t double_indirect;  ///< Pointer to a double-indirect block (if needed)
    uint16_t permissions;      ///< File permissions (UNIX style: rwxrwxrwx)
    uint16_t links_count;      ///< Number of links pointing to this inode
    uint32_t owner_id;         ///< Owner ID
    uint32_t group_id;         ///< Group ID
    uint64_t created_at;       ///< Creation timestamp
    uint64_t modified_at;      ///< Last modification timestamp
    uint64_t accessed_at;      ///< Last access timestamp
    bool is_directory;         ///< Indicates whether the inode represents a directory
    bool is_used; ///< Indicates if the inode is allocated/used
} Inode;

/**
 * @struct Directory
 * @brief Represents a directory in the file system.
 * 
 * @details A directory contains entries for files and subdirectories, including
 *          their names and corresponding inode indices.
 */
typedef struct Directory {
    uint32_t parent_inode;    ///< Inode of the parent directory
    uint32_t entries[128];    ///< Inodes of files or subdirectories
    char names[128][256];     ///< Names of files or subdirectories
    uint32_t entry_count;     ///< Number of entries in the directory
} Directory;

/**
 * @struct Superblock
 * @brief Represents the superblock of the partition.
 * 
 * @details The superblock contains global metadata about the partition, such as
 *          its size, block size, and the number of free blocks.
 */
typedef struct Superblock {
    char magic[8];            ///< Unique identifier for the file system
    uint32_t total_size;      ///< Total size of the partition
    uint32_t block_size;      ///< Size of a block
    uint32_t total_blocks;    ///< Total number of blocks
    uint32_t free_blocks;     ///< Number of free blocks
    uint32_t file_table_start;///< Start of the file table (not used here)
    uint32_t data_start;      ///< Start of the data blocks
} Superblock;

/**
 * @struct Partition
 * @brief Represents the raw partition.
 * 
 * @details The partition structure manages the raw storage, including the bitmap
 *          for tracking block usage and the array of blocks.
 */
typedef struct Partition {
    uint32_t total_size;      ///< Total size of the partition (in bytes)
    uint32_t block_size;      ///< Size of a block (in bytes)
    uint32_t total_blocks;    ///< Total number of blocks
    uint32_t free_blocks;     ///< Number of free blocks
    uint8_t *bitmap;          ///< Bitmap to track used and free blocks
    Block *blocks;            ///< Array of blocks
    void *data;               ///< Pointer to the raw partition data
} Partition;

/**
 * @struct FileDescriptor
 * @brief Represents an open file in the file system.
 * 
 * @details This structure tracks the state of an open file, including its current
 *          position, the associated inode, and other metadata.
 */
typedef struct FileDescriptor {
    uint32_t fd_id;          ///< Unique file descriptor ID
    uint32_t inode_id;       ///< Inode of the opened file
    uint32_t current_pos;    ///< Current read/write position (offset in bytes)
    uint16_t mode;           ///< Opening mode (e.g., read-only, write-only, read-write)
    bool is_used;            ///< Indicates if the descriptor is currently in use
} FileDescriptor;

/**
 * @struct FileSystem
 * @brief Represents the global file system structure.
 * 
 * @details This structure combines all components of the file system, including
 *          the superblock, inode table, and partition.
 */
typedef struct FileSystem {
    Superblock superblock;    ///< Superblock of the partition
    Inode *inode_table;       ///< Table of inodes
    Partition partition;      ///< Raw partition
    FileDescriptor *open_files_table; ///< Table of open file descriptors
    uint32_t max_open_files;  ///< Maximum number of files that can be opened simultaneously
} FileSystem;

/**
 * @brief Initializes a new partition and saves it to a file.
 * 
 * @param fs Pointer to the FileSystem structure
 * @param img_path Path to the file where the partition will be saved
 * @param total_size Total size of the partition (in bytes)
 * @param block_size Size of each block (in bytes)
 */
void init_partition(FileSystem *fs, const char *img_path, uint32_t total_size, uint32_t block_size);

/**
 * @brief Loads an existing partition from a file.
 * 
 * @param fs Pointer to the FileSystem structure
 * @param img_path Path to the file containing the partition
 */
void load_partition(FileSystem *fs, const char *img_path);

/**
 * @brief Allocates a free block in the partition.
 * 
 * @param fs Pointer to the FileSystem structure
 * @return Index of the allocated block, or -1 if no blocks are available
 */
int allocate_block(FileSystem *fs);

/**
 * @brief Frees a block in the partition.
 * 
 * @param fs Pointer to the FileSystem structure
 * @param block_index Index of the block to free
 */
void free_block(FileSystem *fs, uint32_t block_index);

/**
 * @brief Checks if a block is free.
 * 
 * @param fs Pointer to the FileSystem structure
 * @param block_index Index of the block to check
 * @return true if the block is free, false otherwise
 */
bool is_block_free(FileSystem *fs, uint32_t block_index);

/**
 * @brief Trouve l'inode correspondant à un chemin donné
 *
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin du fichier (ex: "/dir1/file.txt")
 * @return uint32_t Numéro de l'inode trouvé, ou (uint32_t)-1 si non trouvé
 */
uint32_t find_inode_by_path(FileSystem *fs, const char *path);

/**
 * @brief Lit le contenu d'un répertoire dans une structure Directory
 *
 * @param fs Pointeur vers le système de fichiers
 * @param inode_num Numéro de l'inode du répertoire
 * @param dir Pointeur vers la structure Directory à remplir
 * @return true en cas de succès, false en cas d'échec
 */
bool read_directory(FileSystem *fs, uint32_t inode_num, Directory *dir);
/**
 * @brief Crée un nouveau fichier dans le système de fichiers
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin complet du nouveau fichier
 * @param mode Mode et permissions du fichier
 * @return uint32_t Numéro d'inode du fichier créé, ou (uint32_t)-1 en cas d'erreur
 */
uint32_t create_file(FileSystem *fs, const char *path, uint16_t mode);

/**
 * @brief Crée un nouveau répertoire dans le système de fichiers
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin complet du nouveau répertoire
 * @param mode Permissions du répertoire
 * @return uint32_t Numéro d'inode du répertoire créé, ou (uint32_t)-1 en cas d'erreur
 */
uint32_t create_directory(FileSystem *fs, const char *path, uint16_t mode);

/**
 * @brief Sépare un chemin en répertoire parent et nom de fichier
 * 
 * @param full_path Chemin complet
 * @param parent_path Buffer pour le chemin du parent
 * @param filename Buffer pour le nom du fichier
 * @return true si succès, false si échec
 */
bool split_path(const char *full_path, char *parent_path, char *filename);


/**
 * @brief Alloue un nouvel inode libre
 * 
 * @param fs Pointeur vers le système de fichiers
 * @return uint32_t Numéro d'inode alloué, ou (uint32_t)-1 si erreur
 */
uint32_t allocate_inode(FileSystem *fs);

/**
 * @brief Initialise un nouvel inode
 * 
 * @param inode Pointeur vers l'inode à initialiser
 * @param id Numéro d'inode
 * @param permissions Permissions du fichier
 * @param is_directory Si true, crée un répertoire
 */
void init_inode(Inode *inode, uint32_t id, uint16_t permissions, bool is_directory);

/**
 * @brief Ajoute une entrée dans un répertoire
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param dir_inode Inode du répertoire
 * @param entry_inode Inode à ajouter
 * @param name Nom de l'entrée
 * @return true si succès, false si échec
 */
bool add_directory_entry(FileSystem *fs, uint32_t dir_inode, uint32_t entry_inode, const char *name);

/**
 * @brief Libère un inode
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param inode_num Numéro d'inode à libérer
 */
void free_inode(FileSystem *fs, uint32_t inode_num);

/**
 * @brief Trouve un fichier dans un répertoire
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param dir_inode Inode du répertoire
 * @param name Nom du fichier à trouver
 * @return uint32_t Inode du fichier trouvé, ou (uint32_t)-1 si non trouvé
 */
uint32_t find_file_in_directory(FileSystem *fs, uint32_t dir_inode, const char *name);

int truncate_file(FileSystem *fs, uint32_t id);

/**
 * @brief Écrit le contenu d'un répertoire dans les blocs de l'inode
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param dir_inode Inode du répertoire à écrire
 * @param dir Pointeur vers la structure Directory à écrire
 * @return true en cas de succès, false en cas d'échec
 */
bool write_directory(FileSystem *fs, uint32_t dir_inode, Directory *dir);

/**
 * @brief Garantit que l'inode a assez de blocs alloués
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param inode Pointeur vers l'inode
 * @param blocks_needed Nombre de blocs nécessaires
 * @return true si les blocs sont disponibles, false sinon
 */
bool ensure_inode_blocks(FileSystem *fs, Inode *inode, uint32_t blocks_needed);

/**
 * @brief Écrit des données dans les blocs d'un inode
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param inode Pointeur vers l'inode
 * @param buffer Données à écrire
 * @param size Taille des données
 * @param offset Offset dans le fichier
 * @return true en cas de succès, false en cas d'échec
 */
bool write_inode_data(FileSystem *fs, Inode *inode, uint8_t *buffer, uint32_t size, uint32_t offset);
/**
 * @brief Écrit des données dans un seul bloc
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param block_num Numéro du bloc
 * @param buffer Données à écrire
 * @param size Nombre d'octets à écrire
 * @param offset Offset dans le bloc
 * @return true en cas de succès, false en cas d'échec
 */
bool write_single_block(FileSystem *fs, uint32_t block_num, uint8_t *buffer, 
    uint32_t size, uint32_t offset);

    int allocate_indirect_block(FileSystem *fs);
    int allocate_block_for_inode(FileSystem *fs, Inode *inode, uint32_t logical_block);
    uint32_t get_indirect_block(FileSystem *fs, uint32_t indirect_blk, uint32_t idx);
    
#endif // PARTITION_H