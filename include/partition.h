#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>
#include <stdbool.h>

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

#define MAX_FILES 1024 ///< Maximum number of files in the inode table

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

#endif // PARTITION_H