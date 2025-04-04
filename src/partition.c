/**
 * @file partition.c
 * @brief This file contains the functions to create and manage partitions.
 * 
 * @details This module defines the structures and functions required to manage
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

#include "partition.h" // Inclure les déclarations des structures et fonctions

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MAX_FILES 1024 ///< Maximum number of files in the inode table

/**
 * @brief Initializes a new partition and saves it to a file.
 * 
 * @param fs Pointer to the FileSystem structure
 * @param img_path Path to the file where the partition will be saved
 * @param total_size Total size of the partition (in bytes)
 * @param block_size Size of each block (in bytes)
 */
void init_partition(FileSystem *fs, const char *img_path, uint32_t total_size, uint32_t block_size) {
    // Calculer le nombre total de blocs
    uint32_t total_blocks = total_size / block_size;

    // Initialiser le superblock
    strncpy(fs->superblock.magic, "MYFS", 8);
    fs->superblock.total_size = total_size;
    fs->superblock.block_size = block_size;
    fs->superblock.total_blocks = total_blocks;
    fs->superblock.free_blocks = total_blocks;
    fs->superblock.file_table_start = sizeof(Superblock); // La table des fichiers commence après le superblock
    fs->superblock.data_start = fs->superblock.file_table_start + (sizeof(Inode) * MAX_FILES);

    // Initialiser le bitmap
    fs->partition.bitmap = calloc(total_blocks, sizeof(uint8_t));

    // Initialiser les blocs
    fs->partition.blocks = malloc(total_blocks * sizeof(Block));
    for (uint32_t i = 0; i < total_blocks; i++) {
        fs->partition.blocks[i].index = i;
        fs->partition.blocks[i].data = NULL;
        fs->partition.blocks[i].is_free = true;
    }

    // Initialiser la table des inodes
    fs->inode_table = calloc(MAX_FILES, sizeof(Inode));

    // Initialiser l'inode du répertoire racine
    fs->inode_table[0].id = 0;
    fs->inode_table[0].size = 0;
    fs->inode_table[0].permissions = 0755; // rwxr-xr-x
    fs->inode_table[0].links_count = 1;
    fs->inode_table[0].is_directory = true;
    fs->inode_table[0].created_at = time(NULL);
    fs->inode_table[0].modified_at = time(NULL);
    fs->inode_table[0].accessed_at = time(NULL);

    // Initialiser la structure Directory pour le répertoire racine
    Directory *root_dir = malloc(sizeof(Directory));
    root_dir->parent_inode = 0; // Le parent du répertoire racine est lui-même
    root_dir->entry_count = 0;
    memset(root_dir->entries, 0, sizeof(root_dir->entries));
    memset(root_dir->names, 0, sizeof(root_dir->names));

    // Associer la structure Directory au premier bloc
    fs->partition.blocks[0].data = (uint8_t *)root_dir;
    fs->partition.blocks[0].is_free = false;

    // Sauvegarder la partition dans un fichier .img
    FILE *img_file = fopen(img_path, "wb");
    if (!img_file) {
        perror("Erreur lors de la création du fichier .img");
        exit(EXIT_FAILURE);
    }

    // Écrire le superblock
    fwrite(&fs->superblock, sizeof(Superblock), 1, img_file);

    // Écrire la table des inodes
    fwrite(fs->inode_table, sizeof(Inode), MAX_FILES, img_file);

    // Écrire le bitmap
    fwrite(fs->partition.bitmap, sizeof(uint8_t), total_blocks, img_file);

    // Écrire les blocs de données (initialement vides)
    uint8_t *empty_block = calloc(1, block_size);
    for (uint32_t i = 0; i < total_blocks; i++) {
        fwrite(empty_block, block_size, 1, img_file);
    }
    free(empty_block);

    fclose(img_file);

    printf("Partition initialisée et sauvegardée dans '%s'.\n", img_path);
}

/**
 * @brief Loads an existing partition from a file.
 * 
 * @param fs Pointer to the FileSystem structure
 * @param img_path Path to the file containing the partition
 */
void load_partition(FileSystem *fs, const char *img_path) {
    FILE *img_file = fopen(img_path, "rb");
    if (!img_file) {
        perror("Erreur lors de l'ouverture du fichier .img");
        exit(EXIT_FAILURE);
    }

    // Lire le superblock
    fread(&fs->superblock, sizeof(Superblock), 1, img_file);

    // Allouer et lire la table des inodes
    fs->inode_table = calloc(MAX_FILES, sizeof(Inode));
    fread(fs->inode_table, sizeof(Inode), MAX_FILES, img_file);

    // Allouer et lire le bitmap
    fs->partition.bitmap = calloc(fs->superblock.total_blocks, sizeof(uint8_t));
    fread(fs->partition.bitmap, sizeof(uint8_t), fs->superblock.total_blocks, img_file);

    // Allouer les blocs
    fs->partition.blocks = malloc(fs->superblock.total_blocks * sizeof(Block));
    for (uint32_t i = 0; i < fs->superblock.total_blocks; i++) {
        fs->partition.blocks[i].index = i;
        fs->partition.blocks[i].data = malloc(fs->superblock.block_size);
        fread(fs->partition.blocks[i].data, fs->superblock.block_size, 1, img_file);
        fs->partition.blocks[i].is_free = (fs->partition.bitmap[i] == 0);
    }

    fclose(img_file);

    printf("Partition chargée depuis '%s'.\n", img_path);
}

/**
 * @brief Allocates a free block in the partition.
 * 
 * @param fs Pointer to the FileSystem structure
 * @return Index of the allocated block, or -1 if no blocks are available
 */
int allocate_block(FileSystem *fs) {
    for (uint32_t i = 0; i < fs->superblock.total_blocks; i++) {
        if (fs->partition.bitmap[i] == 0) { // Bloc libre
            fs->partition.bitmap[i] = 1;   // Marquer comme utilisé
            fs->superblock.free_blocks--;  // Réduire le nombre de blocs libres
            fs->partition.blocks[i].is_free = false; // Mettre à jour l'état du bloc
            fs->partition.blocks[i].data = calloc(1, fs->superblock.block_size); // Allouer la mémoire pour le bloc
            return i; // Retourner l'index du bloc alloué
        }
    }
    return -1; // Aucun bloc libre
}

/**
 * @brief Frees a block in the partition.
 * 
 * @param fs Pointer to the FileSystem structure
 * @param block_index Index of the block to free
 */
void free_block(FileSystem *fs, uint32_t block_index) {
    if (block_index < fs->superblock.total_blocks && fs->partition.bitmap[block_index] == 1) {
        fs->partition.bitmap[block_index] = 0; // Marquer comme libre
        fs->superblock.free_blocks++;         // Augmenter le nombre de blocs libres
        free(fs->partition.blocks[block_index].data); // Libérer les données du bloc
        fs->partition.blocks[block_index].data = NULL;
        fs->partition.blocks[block_index].is_free = true;
    }
}

/**
 * @brief Checks if a block is free.
 * 
 * @param fs Pointer to the FileSystem structure
 * @param block_index Index of the block to check
 * @return true if the block is free, false otherwise
 */
bool is_block_free(FileSystem *fs, uint32_t block_index) {
    if (block_index < fs->superblock.total_blocks) {
        return fs->partition.bitmap[block_index] == 0;
    }
    return false; // Index invalide
}
