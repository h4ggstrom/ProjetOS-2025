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

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief Initializes a new partition and saves it to a file.
 *
 * @param fs Pointer to the FileSystem structure
 * @param img_path Path to the file where the partition will be saved
 * @param total_size Total size of the partition (in bytes)
 * @param block_size Size of each block (in bytes)
 */
void init_partition(FileSystem *fs, const char *img_path, uint32_t total_size, uint32_t block_size)
{
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
    for (uint32_t i = 0; i < total_blocks; i++)
    {
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
    fs->inode_table[0].is_used = 1;

     // Initialisation de la table des fichiers ouverts
     fs->max_open_files = MAX_OPEN_FILES; 
     fs->open_files_table = malloc(fs->max_open_files * sizeof(FileDescriptor));
     if (!fs->open_files_table) {
         perror("Erreur d'allocation pour open_files_table");
         exit(EXIT_FAILURE);
     }
 
     // Initialiser tous les descripteurs comme non utilisés
     for (uint32_t i = 0; i < fs->max_open_files; i++) {
         fs->open_files_table[i].is_used = false;  // ou = 0
         fs->open_files_table[i].fd_id = -1;
         fs->open_files_table[i].inode_id = (uint32_t)-1;
         fs->open_files_table[i].current_pos = 0;
         fs->open_files_table[i].mode = 0;
     }


    // Initialiser la structure Directory pour le répertoire racine
    Directory *root_dir = malloc(sizeof(Directory));
    root_dir->parent_inode = 0; // Le parent du répertoire racine est lui-même
    root_dir->entry_count = 0;
    fs->current_directory = 0;
    strncpy(fs->current_path, "/", MAX_PATH_LEN);
    memset(root_dir->entries, 0, sizeof(root_dir->entries));
    memset(root_dir->names, 0, sizeof(root_dir->names));

    // Associer la structure Directory au premier bloc
    fs->partition.blocks[0].data = (uint8_t *)root_dir;
    fs->partition.blocks[0].is_free = false;

    // Sauvegarder la partition dans un fichier .img
    FILE *img_file = fopen(img_path, "wb");
    if (!img_file)
    {
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
    for (uint32_t i = 0; i < total_blocks; i++)
    {
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
void load_partition(FileSystem *fs, const char *img_path)
{
    FILE *img_file = fopen(img_path, "rb");
    if (!img_file)
    {
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
    for (uint32_t i = 0; i < fs->superblock.total_blocks; i++)
    {
        fs->partition.blocks[i].index = i;
        fs->partition.blocks[i].data = malloc(fs->superblock.block_size);
        fs->partition.block_size = fs->superblock.block_size;
        fs->partition.total_blocks = fs->superblock.total_blocks;
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
int allocate_block(FileSystem *fs)
{
    if (!fs || !fs->partition.bitmap)
        return -1;

    uint32_t total_bytes = (fs->superblock.total_blocks + 7) / 8;

    for (uint32_t byte = 0; byte < total_bytes; byte++)
    {
        if (fs->partition.bitmap[byte] != 0xFF)
        { // Au moins un bit libre dans ce byte
            for (uint8_t bit = 0; bit < 8; bit++)
            {
                uint32_t block_idx = byte * 8 + bit;
                if (block_idx >= fs->superblock.total_blocks)
                    break;

                if (!(fs->partition.bitmap[byte] & (1 << bit)))
                {
                    // Marquer le bloc comme utilisé
                    fs->partition.bitmap[byte] |= (1 << bit);
                    fs->superblock.free_blocks--;
                    fs->partition.blocks[block_idx].is_free = false;
                    return block_idx;
                }
            }
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
void free_block(FileSystem *fs, uint32_t block_index)
{
    if (block_index < fs->superblock.total_blocks && fs->partition.bitmap[block_index] == 1)
    {
        fs->partition.bitmap[block_index] = 0;        // Marquer comme libre
        fs->superblock.free_blocks++;                 // Augmenter le nombre de blocs libres
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
bool is_block_free(FileSystem *fs, uint32_t block_index)
{
    if (block_index < fs->superblock.total_blocks)
    {
        return fs->partition.bitmap[block_index] == 0;
    }
    return false; // Index invalide
}

/**
 * @brief Lit des données depuis un seul bloc
 *
 * @param fs Pointeur vers le système de fichiers
 * @param block_num Numéro du bloc à lire
 * @param buffer Buffer de destination
 * @param size Nombre d'octets à lire
 * @param offset Offset dans le bloc
 * @return true en cas de succès, false en cas d'échec
 */
bool read_single_block(FileSystem *fs, uint32_t block_num, uint8_t *buffer,
                       uint32_t size, uint32_t offset)
{
    if (!fs || !buffer || block_num >= fs->partition.total_blocks ||
        offset + size > fs->partition.block_size)
    {
        return false;
    }

    Block *block = &fs->partition.blocks[block_num];
    if (!block || block->is_free)
    {
        return false;
    }

    memcpy(buffer, block->data + offset, size);
    return true;
}

/**
 * @brief Lit les données d'un inode depuis plusieurs blocs
 *
 * @param fs Pointeur vers le système de fichiers
 * @param inode Pointeur vers l'inode
 * @param buffer Buffer de destination
 * @param size Taille à lire
 * @param offset Offset de départ dans le fichier
 * @return true en cas de succès, false en cas d'échec
 */
bool read_inode_data(FileSystem *fs, Inode *inode, uint8_t *buffer, uint32_t size, uint32_t offset)
{
    if (!fs || !inode || !buffer || offset > inode->size)
    {
        return false;
    }

    // Calculer la taille effective à lire
    uint32_t remaining_bytes = inode->size - offset;
    if (size > remaining_bytes)
    {
        size = remaining_bytes;
    }
    if (size == 0)
    {
        return true;
    }

    uint32_t bytes_read = 0;
    uint32_t current_pos = offset;
    uint32_t block_size = fs->partition.block_size;

    // 1. Lire les blocs directs
    for (int i = 0; i < 12 && bytes_read < size; i++)
    {
        if (inode->blocks[i] == 0)
        {
            continue;
        }

        uint32_t block_offset = current_pos % block_size;
        uint32_t bytes_in_block = block_size - block_offset;
        uint32_t bytes_to_read = (size - bytes_read) < bytes_in_block ? (size - bytes_read) : bytes_in_block;

        if (current_pos / block_size == i)
        {
            if (!read_single_block(fs, inode->blocks[i],
                                   buffer + bytes_read,
                                   bytes_to_read,
                                   block_offset))
            {
                return false;
            }
            bytes_read += bytes_to_read;
            current_pos += bytes_to_read;
        }
    }

    // 2. Lire le bloc indirect simple si nécessaire
    if (bytes_read < size && inode->indirect_block != 0)
    {
        uint32_t indirect_block_pointers[block_size / sizeof(uint32_t)];
        if (!read_single_block(fs, inode->indirect_block,
                               (uint8_t *)indirect_block_pointers,
                               block_size, 0))
        {
            return false;
        }

        uint32_t start_block = 12;
        uint32_t end_block = start_block + (block_size / sizeof(uint32_t));

        for (uint32_t i = start_block; i < end_block && bytes_read < size; i++)
        {
            if (indirect_block_pointers[i - start_block] == 0)
            {
                continue;
            }

            uint32_t block_offset = current_pos % block_size;
            uint32_t bytes_in_block = block_size - block_offset;
            uint32_t bytes_to_read = (size - bytes_read) < bytes_in_block ? (size - bytes_read) : bytes_in_block;

            if (current_pos / block_size == i)
            {
                if (!read_single_block(fs, indirect_block_pointers[i - start_block],
                                       buffer + bytes_read,
                                       bytes_to_read,
                                       block_offset))
                {
                    return false;
                }
                bytes_read += bytes_to_read;
                current_pos += bytes_to_read;
            }
        }
    }

    // 3. Lire le bloc doublement indirect si nécessaire
    if (bytes_read < size && inode->double_indirect != 0)
    {
        uint32_t double_indirect_block_pointers[block_size / sizeof(uint32_t)];
        if (!read_single_block(fs, inode->double_indirect,
                               (uint8_t *)double_indirect_block_pointers,
                               block_size, 0))
        {
            return false;
        }

        uint32_t pointers_per_block = block_size / sizeof(uint32_t);
        uint32_t start_block = 12 + (block_size / sizeof(uint32_t));
        uint32_t block_index = start_block;

        for (uint32_t i = 0; i < pointers_per_block && bytes_read < size; i++)
        {
            if (double_indirect_block_pointers[i] == 0)
            {
                block_index += pointers_per_block;
                continue;
            }

            uint32_t indirect_block_pointers[pointers_per_block];
            if (!read_single_block(fs, double_indirect_block_pointers[i],
                                   (uint8_t *)indirect_block_pointers,
                                   block_size, 0))
            {
                return false;
            }

            for (uint32_t j = 0; j < pointers_per_block && bytes_read < size; j++)
            {
                if (indirect_block_pointers[j] == 0)
                {
                    block_index++;
                    continue;
                }

                uint32_t block_offset = current_pos % block_size;
                uint32_t bytes_in_block = block_size - block_offset;
                uint32_t bytes_to_read = (size - bytes_read) < bytes_in_block ? (size - bytes_read) : bytes_in_block;

                if (current_pos / block_size == block_index)
                {
                    if (!read_single_block(fs, indirect_block_pointers[j],
                                           buffer + bytes_read,
                                           bytes_to_read,
                                           block_offset))
                    {
                        return false;
                    }
                    bytes_read += bytes_to_read;
                    current_pos += bytes_to_read;
                }
                block_index++;
            }
        }
    }

    return true;//bytes_read == size;
}

/**
 * @brief Lit le contenu d'un répertoire dans une structure Directory
 *
 * @param fs Pointeur vers le système de fichiers
 * @param inode_num Numéro de l'inode du répertoire
 * @param dir Pointeur vers la structure Directory à remplir
 * @return true en cas de succès, false en cas d'échec
 */
bool read_directory(FileSystem *fs, uint32_t inode_num, Directory *dir)
{
    if (!fs || inode_num >= MAX_FILES || !dir || !fs->inode_table[inode_num].is_directory)
    {
        return false;
    }

    Inode *inode = &fs->inode_table[inode_num];
    uint8_t *buffer = malloc(inode->size);
    if (!buffer)
    {
        return false;
    }

    // Lire les données du répertoire depuis les blocs
    if (!read_inode_data(fs, inode, buffer, inode->size, 0))
    {
        free(buffer);
        return false;
    }

    // Désérialiser la structure Directory
    memcpy(dir, buffer, sizeof(Directory));
    free(buffer);
    return true;
}

/**
 * @brief Trouve l'inode correspondant à un chemin donné
 *
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin du fichier (ex: "/dir1/file.txt")
 * @return uint32_t Numéro de l'inode trouvé, ou (uint32_t)-1 si non trouvé
 */
uint32_t find_inode_by_path(FileSystem *fs, const char *path)
{
    // Vérification des paramètres
    if (!fs || !path || path[0] != '/')
    {
        return (uint32_t)-1;
    }

    // Cas spécial pour la racine
    if (strcmp(path, "/") == 0)
    {
        return 0; // On suppose que l'inode 0 est le répertoire racine
    }

    // Copie du chemin pour éviter de modifier l'original
    char *path_copy = strdup(path);
    if (!path_copy)
    {
        return (uint32_t)-1;
    }

    uint32_t current_inode = 0; // On commence à la racine (inode 0)
    char *token = strtok(path_copy, "/");
    char *next_token = NULL;

    while (token != NULL)
    {
        // Vérifier que current_inode est un répertoire
        if (current_inode >= MAX_FILES || !fs->inode_table[current_inode].is_directory)
        {
            free(path_copy);
            return (uint32_t)-1;
        }

        // Lire le répertoire
        Directory dir;
        if (!read_directory(fs, current_inode, &dir))
        {
            free(path_copy);
            return (uint32_t)-1;
        }

        // Vérifier si le token existe dans le répertoire
        uint32_t found_inode = (uint32_t)-1;
        for (uint32_t i = 0; i < dir.entry_count; i++)
        {
            if (strcmp(dir.names[i], token) == 0)
            {
                found_inode = dir.entries[i];
                break;
            }
        }

        if (found_inode == (uint32_t)-1)
        {
            free(path_copy);
            return (uint32_t)-1;
        }

        // Préparer la prochaine itération
        current_inode = found_inode;
        next_token = strtok(NULL, "/");

        // Si c'est le dernier token et qu'on a trouvé, on retourne l'inode
        if (next_token == NULL)
        {
            free(path_copy);
            return current_inode;
        }

        token = next_token;
    }

    free(path_copy);
    return (uint32_t)-1;
}

/**
 * @brief Ouvre un fichier et retourne un descripteur de fichier
 *
 * @param fs         Pointeur vers le système de fichiers
 * @param path       Chemin du fichier à ouvrir
 * @param mode       Mode d'ouverture (O_RDONLY, O_WRONLY, etc.)
 * @return int       Descripteur de fichier (≥0) en cas de succès, -1 en cas d'erreur
 */
int fs_open_file(FileSystem *fs, const char *path, int mode)
{
    // 1. Trouver l'inode correspondant au chemin
    uint32_t inode_id = find_inode_by_path(fs, path);

    // Si le fichier n'existe pas et que O_CREAT est spécifié, le créer
    if (inode_id == (uint32_t)-1 && (mode & O_CREAT))
    {
        // inode_id = create_file(fs, path);
        if (inode_id == (uint32_t)-1)
        {
            return -1; // Échec de la création
        }
    }
    else if (inode_id == (uint32_t)-1)
    {
        return -1; // Fichier non trouvé
    }

    // 2. Vérifier les permissions
    Inode *inode = &fs->inode_table[inode_id];
    if ((mode & O_WRONLY || mode & O_RDWR) && !(inode->permissions & 0222))
    {
        return -1; // Pas les permissions en écriture
    }
    if ((mode & O_RDONLY || mode & O_RDWR) && !(inode->permissions & 0444))
    {
        return -1; // Pas les permissions en lecture
    }

    // 3. Tronquer le fichier si O_TRUNC est spécifié
    if (mode & O_TRUNC)
    {
        // truncate_file(fs, inode_id);
    }

    // 4. Trouver un slot libre dans la table des descripteurs
    int fd = -1;
    for (uint32_t i = 0; i < fs->max_open_files; i++)
    {
        if (!fs->open_files_table[i].is_used)
        {
            fd = i;
            break;
        }
    }
    if (fd == -1)
    {
        return -1; // Trop de fichiers ouverts
    }

    // 5. Initialiser le descripteur de fichier
    fs->open_files_table[fd].fd_id = fd;
    fs->open_files_table[fd].inode_id = inode_id;
    fs->open_files_table[fd].current_pos = 0;
    fs->open_files_table[fd].mode = mode;
    fs->open_files_table[fd].is_used = true;

    // 6. Mettre à jour le timestamp d'accès
    inode->accessed_at = (uint64_t)time(NULL);

    return fd;
}

/**
 * @brief Ferme un fichier précédemment ouvert
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param fd Descripteur de fichier à fermer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int fs_close_file(FileSystem *fs, int fd) {
    // 1. Vérifications de base
    if (!fs || fd < 0 || fd >= fs->max_open_files) {
        fprintf(stderr, "Descripteur de fichier invalide\n");
        return -1;
    }

    // 2. Vérifier que le fichier est bien ouvert
    if (!fs->open_files_table[fd].is_used) {
        fprintf(stderr, "Le fichier n'est pas ouvert\n");
        return -1;
    }

    // 3. Mettre à jour le timestamp de modification si le fichier était ouvert en écriture
    uint32_t inode_id = fs->open_files_table[fd].inode_id;
    if (inode_id < MAX_FILES) {
        Inode *inode = &fs->inode_table[inode_id];
        
        // Si le fichier était ouvert en mode écriture
        if (fs->open_files_table[fd].mode & (O_WRONLY | O_RDWR)) {
            inode->modified_at = (uint64_t)time(NULL);
        }
    }

    // 4. Libérer le descripteur de fichier
    fs->open_files_table[fd].is_used = false;
    fs->open_files_table[fd].fd_id = -1;
    fs->open_files_table[fd].inode_id = (uint32_t)-1;
    fs->open_files_table[fd].current_pos = 0;
    fs->open_files_table[fd].mode = 0;

    return 0;
}

/**
 * @brief Crée un nouveau fichier dans le système de fichiers
 *
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin complet du nouveau fichier
 * @param mode Mode et permissions du fichier
 * @return uint32_t Numéro d'inode du fichier créé, ou (uint32_t)-1 en cas d'erreur
 */
uint32_t create_file(FileSystem *fs, const char *path, uint16_t mode)
{
    // 1. Vérifier les paramètres
    if (!fs || !path || path[0] != '/')
    {
        return (uint32_t)-1;
    }

    // 2. Extraire le répertoire parent et le nom du fichier
    char parent_path[MAX_PATH_LEN];
    char filename[MAX_FILENAME_LEN];
    if (!split_path(path, parent_path, filename))
    {
        return (uint32_t)-1;
    }

    // 3. Trouver l'inode du répertoire parent
    uint32_t parent_inode = find_inode_by_path(fs, parent_path);
    if (parent_inode == (uint32_t)-1)
    {
        return (uint32_t)-1;
    }

    // 4. Vérifier que le fichier n'existe pas déjà
    if (find_file_in_directory(fs, parent_inode, filename) != (uint32_t)-1)
    {
        return (uint32_t)-1; // Fichier existe déjà
    }

    // 5. Allouer un nouvel inode
    uint32_t new_inode = allocate_inode(fs);
    if (new_inode == (uint32_t)-1)
    {
        return (uint32_t)-1;
    }

    // 6. Initialiser le nouvel inode
    Inode *inode = &fs->inode_table[new_inode];
    init_inode(inode, new_inode, mode & ~S_IFMT, false); // S_IFMT est le masque pour le type

    // 7. Ajouter l'entrée dans le répertoire parent
    if (!add_directory_entry(fs, parent_inode, new_inode, filename))
    {
        free_inode(fs, new_inode);
        return (uint32_t)-1;
    }

    return new_inode;
}

/**
 * @brief Sépare un chemin en répertoire parent et nom de fichier
 *
 * @param full_path Chemin complet
 * @param parent_path Buffer pour le chemin du parent
 * @param filename Buffer pour le nom du fichier
 * @return true si succès, false si échec
 */
bool split_path(const char *full_path, char *parent_path, char *filename)
{
    if (!full_path || !parent_path || !filename)
    {
        return false;
    }

    // Trouver le dernier '/'
    const char *last_slash = strrchr(full_path, '/');
    if (!last_slash)
    {
        return false;
    }

    // Copier le parent path
    size_t parent_len = last_slash - full_path;
    if (parent_len == 0)
    {
        strcpy(parent_path, "/"); // Cas racine
    }
    else
    {
        strncpy(parent_path, full_path, parent_len);
        parent_path[parent_len] = '\0';
    }

    // Copier le filename
    strncpy(filename, last_slash + 1, MAX_FILENAME_LEN - 1);
    filename[MAX_FILENAME_LEN - 1] = '\0';

    return true;
}

/**
 * @brief Alloue un nouvel inode libre
 *
 * @param fs Pointeur vers le système de fichiers
 * @return uint32_t Numéro d'inode alloué, ou (uint32_t)-1 si erreur
 */

uint32_t allocate_inode(FileSystem *fs)
{
    if (!fs)
        return (uint32_t)-1;
    for (uint32_t i = 0; i < MAX_FILES; i++)
    {
        // Vérifie à la fois le flag is_used ET l'id
        if (!fs->inode_table[i].is_used && fs->inode_table[i].id == 0)
        {
            // Marque l'inode comme utilisé
            fs->inode_table[i].is_used = true;
            fs->inode_table[i].id = i; // ID correspond à l'index
            return i;
        }
    }
    return (uint32_t)-1; // Plus d'inodes disponibles
}

/**
 * @brief Initialise un nouvel inode
 *
 * @param inode Pointeur vers l'inode à initialiser
 * @param id Numéro d'inode
 * @param permissions Permissions du fichier
 * @param is_directory Si true, crée un répertoire
 */
void init_inode(Inode *inode, uint32_t id, uint16_t permissions, bool is_directory)
{
    memset(inode, 0, sizeof(Inode));
    inode->id = id;
    inode->permissions = permissions | (is_directory ? S_IFDIR : S_IFREG);
    inode->links_count = 1;
    inode->owner_id = 0; // Root par défaut
    inode->group_id = 0; // Root group par défaut
    inode->created_at = time(NULL);
    inode->modified_at = inode->created_at;
    inode->accessed_at = inode->created_at;
    inode->is_directory = is_directory;
    inode->is_used = 0;
}

/**
 * @brief Ajoute une entrée dans un répertoire
 *
 * @param fs Pointeur vers le système de fichiers
 * @param dir_inode Inode du répertoire
 * @param entry_inode Inode à ajouter
 * @param name Nom de l'entrée
 * @return true si succès, false si échec
 */
bool add_directory_entry(FileSystem *fs, uint32_t dir_inode, uint32_t entry_inode, const char *name)
{
    // 1. Vérifications
    if (!fs || dir_inode >= MAX_FILES || entry_inode >= MAX_FILES || !name || strlen(name) >= MAX_FILENAME_LEN)
    {
        return false;
    }

    Inode *dir_inode_ptr = &fs->inode_table[dir_inode];
    if (!dir_inode_ptr->is_directory)
    {
        return false;
    }

    // 2. Charger le répertoire
    Directory dir;
    if (!read_directory(fs, dir_inode, &dir))
    {
        return false;
    }

    // 3. Vérifier si l'entrée existe déjà
    for (uint32_t i = 0; i < dir.entry_count; i++)
    {
        if (strcmp(dir.names[i], name) == 0)
        {
            return false; // Entrée déjà existante
        }
    }

    // 4. Vérifier la capacité
    if (dir.entry_count >= DIR_ENTRIES_LIMIT)
    {
        return false;
    }

    // 5. Ajouter la nouvelle entrée
    dir.entries[dir.entry_count] = entry_inode;
    strncpy(dir.names[dir.entry_count], name, MAX_FILENAME_LEN - 1);
    dir.names[dir.entry_count][MAX_FILENAME_LEN - 1] = '\0'; // Sécurité
    dir.entry_count++;

    // 6. Sauvegarder
    if (!write_directory(fs, dir_inode, &dir))
    {
        return false;
    }

    // 7. Mettre à jour les métadonnées
    dir_inode_ptr->modified_at = time(NULL);
    dir_inode_ptr->size += sizeof(Directory); // Taille approximative

    return true;
}

/**
 * @brief Libère un inode
 *
 * @param fs Pointeur vers le système de fichiers
 * @param inode_num Numéro d'inode à libérer
 */
void free_inode(FileSystem *fs, uint32_t inode_num)
{
    // 1. Vérifications de sécurité
    if (!fs || inode_num >= MAX_FILES)
    {
        return;
    }

    Inode *inode = &fs->inode_table[inode_num];

    // 3. Gestion des liens (comptage de références)
    if (inode->links_count > 1)
    {
        inode->links_count--;
        return; // On ne libère pas tant qu'il reste des liens
    }

    // 4. Libération des ressources
    // truncate_file(fs, inode_num); // Doit être implémentée

    // 5. Réinitialisation sélective (conserve le numéro)
    inode->is_used = false;
    inode->size = 0;
    inode->is_directory = false;
    inode->links_count = 0;
    memset(inode->blocks, 0, sizeof(inode->blocks));
    inode->indirect_block = 0;
    inode->double_indirect = 0;
    inode->id = 0;

    // Optionnel : conserver certaines infos pour le débogage
    inode->modified_at = (uint64_t)time(NULL);
}

/**
 * @brief Trouve un fichier dans un répertoire
 *
 * @param fs Pointeur vers le système de fichiers
 * @param dir_inode Inode du répertoire
 * @param name Nom du fichier à trouver
 * @return uint32_t Inode du fichier trouvé, ou (uint32_t)-1 si non trouvé
 */
uint32_t find_file_in_directory(FileSystem *fs, uint32_t dir_inode, const char *name)
{
    Directory dir;
    if (!read_directory(fs, dir_inode, &dir))
    {
        return (uint32_t)-1;
    }

    for (uint32_t i = 0; i < dir.entry_count; i++)
    {
        if (strcmp(dir.names[i], name) == 0)
        {
            return dir.entries[i];
        }
    }

    return (uint32_t)-1;
}

/**
 * @brief Écrit le contenu d'un répertoire dans les blocs de l'inode
 *
 * @param fs Pointeur vers le système de fichiers
 * @param dir_inode Inode du répertoire à écrire
 * @param dir Pointeur vers la structure Directory à écrire
 * @return true en cas de succès, false en cas d'échec
 */
bool write_directory(FileSystem *fs, uint32_t dir_inode, Directory *dir)
{
    // 1. Vérifications de base
    if (!fs || dir_inode >= MAX_FILES || !dir || !fs->inode_table[dir_inode].is_directory)
    {
        return false;
    }

    Inode *inode = &fs->inode_table[dir_inode];
    uint32_t block_size = fs->partition.block_size;
    uint32_t required_size = sizeof(Directory);

    // 2. Calculer le nombre de blocs nécessaires
    uint32_t blocks_needed = (required_size + block_size - 1) / block_size;

    // 3. Vérifier/gérer l'allocation des blocs
    if (!ensure_inode_blocks(fs, inode, blocks_needed))
    {
        return false;
    }

    // 4. Sérialiser la structure Directory dans un buffer
    uint8_t *buffer = malloc(required_size);
    if (!buffer)
    {
        return false;
    }
    memcpy(buffer, dir, required_size);

    // 5. Écrire le buffer dans les blocs de l'inode
    bool success = write_inode_data(fs, inode, buffer, required_size, 0);

    free(buffer);

    // 6. Mettre à jour la taille et les métadonnées de l'inode
    if (success)
    {
        inode->size = required_size;
        inode->modified_at = (uint64_t)time(NULL);
    }

    return success;
}

/**
 * @brief Garantit que l'inode a assez de blocs alloués
 *
 * @param fs Pointeur vers le système de fichiers
 * @param inode Pointeur vers l'inode
 * @param blocks_needed Nombre de blocs nécessaires
 * @return true si les blocs sont disponibles, false sinon
 */
bool ensure_inode_blocks(FileSystem *fs, Inode *inode, uint32_t blocks_needed)
{
    // 1. Vérifications de sécurité
    if (!fs || !inode || blocks_needed > 12)
    { // Maximum 12 blocs directs
        return false;
    }

    // 2. Compter les blocs déjà alloués
    uint32_t current_blocks = 0;
    for (uint32_t i = 0; i < 12; i++)
    {
        if (inode->blocks[i] != 0)
        {
            current_blocks++;
        }
    }

    // 3. Vérifier si allocation nécessaire
    if (current_blocks >= blocks_needed)
    {
        return true;
    }

    // 4. Allouer les nouveaux blocs temporairement
    uint32_t blocks_to_allocate = blocks_needed - current_blocks;
    uint32_t new_blocks[12] = {0};
    uint32_t allocated_count = 0;

    for (uint32_t i = 0; i < blocks_to_allocate; i++)
    {
        int block_num = allocate_block(fs);
        if (block_num == -1)
        {
            // Rollback : libérer les blocs déjà alloués
            for (uint32_t j = 0; j < allocated_count; j++)
            {
                free_block(fs, new_blocks[j]);
            }
            return false;
        }
        new_blocks[allocated_count++] = block_num;
    }

    // 5. Assigner les nouveaux blocs aux slots vides
    for (uint32_t i = 0, assigned = 0; i < 12 && assigned < allocated_count; i++)
    {
        if (inode->blocks[i] == 0)
        {
            inode->blocks[i] = new_blocks[assigned++];
        }
    }

    return true;
}

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
bool write_inode_data(FileSystem *fs, Inode *inode, uint8_t *buffer, uint32_t size, uint32_t offset) {
        // 1. Vérifications initiales renforcées
        if (!fs || !inode || !buffer || offset > inode->size || fs->partition.block_size == 0) {
            return false;
        }
    
        uint32_t block_size = fs->partition.block_size;
        uint32_t bytes_written = 0;
        uint32_t current_pos = offset;

    for (int i = 0; i < 12 && bytes_written < size; i++) {
        uint32_t block_offset = current_pos % block_size;
        uint32_t bytes_available = block_size - block_offset;
        uint32_t bytes_to_write = MIN(size - bytes_written, bytes_available);

        // Allocation dynamique si bloc vide
        if (inode->blocks[i] == 0) {
            int new_block = allocate_block(fs);
            if (new_block == -1) {
                // Optionnel : nettoyer les blocs déjà alloués
                return false; 
            }
            inode->blocks[i] = new_block;
        }
        if(!write_single_block(fs, inode->blocks[i],buffer + bytes_written,bytes_to_write,block_offset)) {
            return false;
        }

        bytes_written += bytes_to_write;
        current_pos += bytes_to_write;
    }

    // Gestion des blocs indirects si besoin (exemple simplifié)
    if (bytes_written < size ) {
        if (inode->indirect_block == NULL){
        inode->indirect_block = allocate_indirect_block(fs);}

        // [...] Écriture dans les blocs indirects
    }

    // Mise à jour de la taille si extension
    if (offset + bytes_written > inode->size) {
        inode->size = offset + bytes_written;
    }

    return bytes_written == size;

    // Met à jour la taille si extension
    if(offset + bytes_written > inode->size)
    {
        inode->size = offset + bytes_written;
    }

    return true;
}

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
                        uint32_t size, uint32_t offset)
{
    if (!fs || !buffer || block_num >= fs->partition.total_blocks ||
        offset + size > fs->partition.block_size)
    {
        return false;
    }

    Block *block = &fs->partition.blocks[block_num];
    if (!block || block->is_free)
    {
        return false;
    }

    memcpy(block->data + offset, buffer, size);
    return true;
}

int allocate_block_for_inode(FileSystem *fs, Inode *inode, uint32_t logical_block)
{
    // Blocs directs (0-11)
    if (logical_block < 12)
    {
        if (inode->blocks[logical_block] == 0)
        {
            int blk = allocate_block(fs);
            if (blk == -1)
                return -1;
            inode->blocks[logical_block] = blk;
        }
        return inode->blocks[logical_block];
    }

    // Blocs indirects (12-1035)
    if (logical_block < 12 + 1024)
    {
        if (inode->indirect_block == 0)
        {
            inode->indirect_block = allocate_indirect_block(fs);
            if (inode->indirect_block == -1)
                return -1;
        }
        return get_indirect_block(fs, inode->indirect_block, logical_block - 12);
    }
    /*
    // Blocs double-indirects (1036+)
    if (inode->double_indirect == 0) {
        inode->double_indirect = allocate_indirect_block(fs);
        if (inode->double_indirect == -1) return -1;
    }
    return get_double_indirect_block(fs, inode->double_indirect, logical_block - 12 - 1024);*/
}

int allocate_indirect_block(FileSystem *fs)
{
    int blk = allocate_block(fs);
    if (blk != -1)
    {
        // Initialise comme tableau de pointeurs
        uint32_t *ptrs = (uint32_t *)fs->partition.blocks[blk].data;
        memset(ptrs, 0, fs->superblock.block_size);
    }
    return blk;
}

uint32_t get_indirect_block(FileSystem *fs, uint32_t indirect_blk, uint32_t idx)
{
    uint32_t *ptrs = (uint32_t *)fs->partition.blocks[indirect_blk].data;
    if (ptrs[idx] == 0)
    {
        ptrs[idx] = allocate_block(fs);
    }
    return ptrs[idx];
}

/**
 * @brief Crée un nouveau répertoire dans le système de fichiers
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin complet du nouveau répertoire
 * @param mode Permissions du répertoire
 * @return uint32_t Numéro d'inode du répertoire créé, ou (uint32_t)-1 en cas d'erreur
 */
uint32_t create_directory(FileSystem *fs, const char *path, uint16_t mode) {
    // 1. Vérifier les paramètres
    if (!fs || !path || path[0] != '/') {
        fprintf(stderr, "Chemin invalide\n");
        return (uint32_t)-1;
    }

    // 2. Extraire le répertoire parent et le nom du répertoire
    char parent_path[MAX_PATH_LEN];
    char dirname[MAX_FILENAME_LEN];
    if (!split_path(path, parent_path, dirname)) {
        fprintf(stderr, "Impossible de diviser le chemin\n");
        return (uint32_t)-1;
    }

    // 3. Trouver l'inode du répertoire parent
    uint32_t parent_inode = find_inode_by_path(fs, parent_path);
    if (parent_inode == (uint32_t)-1) {
        fprintf(stderr, "Répertoire parent non trouvé\n");
        return (uint32_t)-1;
    }

    // 4. Vérifier les permissions du répertoire parent
    Inode *parent = &fs->inode_table[parent_inode];
    if (!(parent->permissions & 0222)) {
        fprintf(stderr, "Permission refusée\n");
        return (uint32_t)-1;
    }

    // 5. Vérifier que le répertoire n'existe pas déjà
    if (find_file_in_directory(fs, parent_inode, dirname) != (uint32_t)-1) {
        fprintf(stderr, "Le répertoire existe déjà\n");
        return (uint32_t)-1;
    }

    // 6. Allouer un nouvel inode pour le répertoire
    uint32_t new_inode = allocate_inode(fs);
    if (new_inode == (uint32_t)-1) {
        fprintf(stderr, "Plus d'inodes disponibles\n");
        return (uint32_t)-1;
    }

    // 7. Initialiser le nouvel inode comme répertoire
    Inode *dir_inode = &fs->inode_table[new_inode];
    init_inode(dir_inode, new_inode, (mode & ~S_IFMT) | 0755, true);
    dir_inode->is_used = true;

    // 8. Créer la structure Directory pour le nouveau répertoire
    Directory *new_dir = malloc(sizeof(Directory));
    if (!new_dir) {
        free_inode(fs, new_inode);
        return (uint32_t)-1;
    }

    memset(new_dir, 0, sizeof(Directory));
    new_dir->parent_inode = parent_inode;
    new_dir->entry_count = 2; // '.' et '..'

    // Ajouter les entrées '.' et '..'
    new_dir->entries[0] = new_inode;       // '.' pointe vers ce répertoire
    strcpy(new_dir->names[0], ".");
    new_dir->entries[1] = parent_inode;    // '..' pointe vers le parent
    strcpy(new_dir->names[1], "..");

    // 9. Allouer un bloc pour stocker la structure Directory
    int block_num = allocate_block(fs);
    if (block_num == -1) {
        free(new_dir);
        free_inode(fs, new_inode);
        return (uint32_t)-1;
    }

    // 10. Associer le bloc à l'inode
    dir_inode->blocks[0] = block_num;
    fs->partition.blocks[block_num].data = (uint8_t*)new_dir;
    fs->partition.blocks[block_num].is_free = false;

    // 11. Ajouter le répertoire dans le parent
    if (!add_directory_entry(fs, parent_inode, new_inode, dirname)) {
        free_block(fs, block_num);
        free_inode(fs, new_inode);
        return (uint32_t)-1;
    }

    // 12. Mettre à jour les métadonnées du parent
    parent->modified_at = time(NULL);
    parent->accessed_at = time(NULL);

    return new_inode;
}

/**
 * @brief Supprime un répertoire vide
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin du répertoire à supprimer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int remove_directory(FileSystem *fs, const char *path) {
    // 1. Vérifications de base
    if (!fs || !path || path[0] != '/') {
        fprintf(stderr, "Chemin invalide\n");
        return -1;
    }

    // 2. Trouver l'inode du répertoire
    uint32_t dir_inode = find_inode_by_path(fs, path);
    if (dir_inode == (uint32_t)-1) {
        fprintf(stderr, "Répertoire non trouvé\n");
        return -1;
    }

    // 3. Vérifier que c'est bien un répertoire
    Inode *inode = &fs->inode_table[dir_inode];
    if (!inode->is_directory) {
        fprintf(stderr, "Ce n'est pas un répertoire\n");
        return -1;
    }

    // 4. Lire le contenu du répertoire
    Directory dir;
    if (!read_directory(fs, dir_inode, &dir)) {
        fprintf(stderr, "Erreur de lecture du répertoire\n");
        return -1;
    }

    // 5. Vérifier que le répertoire est vide (seulement '.' et '..')
    if (dir.entry_count > 2) {
        fprintf(stderr, "Répertoire non vide\n");
        return -1;
    }

    // 6. Trouver le répertoire parent
    uint32_t parent_inode = dir.entries[1]; // '..' pointe vers le parent
    if (parent_inode >= MAX_FILES) {
        fprintf(stderr, "Répertoire parent invalide\n");
        return -1;
    }

    // 7. Vérifier les permissions du parent
    Inode *parent = &fs->inode_table[parent_inode];
    if (!(parent->permissions & 0222)) {
        fprintf(stderr, "Permission refusée\n");
        return -1;
    }

    // 8. Extraire le nom du répertoire à supprimer
    char parent_path[MAX_PATH_LEN];
    char dirname[MAX_FILENAME_LEN];
    if (!split_path(path, parent_path, dirname)) {
        fprintf(stderr, "Impossible d'extraire le nom du répertoire\n");
        return -1;
    }

    // 9. Supprimer l'entrée du répertoire parent
    Directory parent_dir;
    if (!read_directory(fs, parent_inode, &parent_dir)) {
        fprintf(stderr, "Erreur de lecture du répertoire parent\n");
        return -1;
    }

    bool found = false;
    for (uint32_t i = 0; i < parent_dir.entry_count; i++) {
        if (strcmp(parent_dir.names[i], dirname) == 0 && parent_dir.entries[i] == dir_inode) {
            // Décaler les entrées suivantes
            for (uint32_t j = i; j < parent_dir.entry_count - 1; j++) {
                parent_dir.entries[j] = parent_dir.entries[j+1];
                strcpy(parent_dir.names[j], parent_dir.names[j+1]);
            }
            parent_dir.entry_count--;
            found = true;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "Entrée non trouvée dans le répertoire parent\n");
        return -1;
    }

    // 10. Sauvegarder le répertoire parent modifié
    if (!write_directory(fs, parent_inode, &parent_dir)) {
        fprintf(stderr, "Erreur d'écriture du répertoire parent\n");
        return -1;
    }

    // 11. Libérer les ressources du répertoire
    // Libérer le bloc contenant la structure Directory
    if (inode->blocks[0] != 0) {
        free_block(fs, inode->blocks[0]);
    }

    // Libérer l'inode
    free_inode(fs, dir_inode);

    // 12. Mettre à jour les métadonnées du parent
    parent->modified_at = time(NULL);
    parent->accessed_at = time(NULL);

    return 0;
}

/**
 * @brief Supprime un fichier du système de fichiers
 * 
 * @param fs Pointeur vers le système de fichiers
 * @param path Chemin du fichier à supprimer
 * @return int 0 en cas de succès, -1 en cas d'échec
 */
int remove_file(FileSystem *fs, const char *path) {
    // 1. Vérifications de base
    if (!fs || !path) {
        fprintf(stderr, "Paramètres invalides\n");
        return -1;
    }

    // 2. Trouver l'inode du fichier
    uint32_t file_inode = find_inode_by_path(fs, path);
    if (file_inode == (uint32_t)-1 || file_inode >= MAX_FILES) {
        fprintf(stderr, "Fichier non trouvé: %s\n", path);
        return -1;
    }

    // 3. Vérifier que ce n'est pas un répertoire
    Inode *inode = &fs->inode_table[file_inode];
    if (inode->is_directory) {
        fprintf(stderr, "Impossible de supprimer un répertoire avec unlink: %s\n", path);
        return -1;
    }

    // 4. Trouver le répertoire parent et le nom du fichier
    char parent_path[MAX_PATH_LEN];
    char filename[MAX_FILENAME_LEN];
    if (!split_path(path, parent_path, filename)) {
        fprintf(stderr, "Erreur d'analyse du chemin: %s\n", path);
        return -1;
    }

    uint32_t parent_inode = find_inode_by_path(fs, parent_path);
    if (parent_inode == (uint32_t)-1) {
        fprintf(stderr, "Répertoire parent non trouvé pour: %s\n", path);
        return -1;
    }

    // 5. Vérifier les permissions du parent
    Inode *parent = &fs->inode_table[parent_inode];
    if (!(parent->permissions & 0222)) { // Vérifier permission écriture
        fprintf(stderr, "Permission refusée pour: %s\n", path);
        return -1;
    }

    // 6. Supprimer l'entrée du répertoire parent
    Directory dir;
    if (!read_directory(fs, parent_inode, &dir)) {
        fprintf(stderr, "Erreur de lecture du répertoire parent\n");
        return -1;
    }

    bool found = false;
    for (uint32_t i = 0; i < dir.entry_count; i++) {
        if (strcmp(dir.names[i], filename) == 0 && dir.entries[i] == file_inode) {
            // Décaler les entrées suivantes
            for (uint32_t j = i; j < dir.entry_count - 1; j++) {
                dir.entries[j] = dir.entries[j+1];
                strcpy(dir.names[j], dir.names[j+1]);
            }
            dir.entry_count--;
            found = true;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "Entrée non trouvée dans le répertoire parent\n");
        return -1;
    }

    // 7. Sauvegarder le répertoire parent modifié
    if (!write_directory(fs, parent_inode, &dir)) {
        fprintf(stderr, "Erreur d'écriture du répertoire parent\n");
        return -1;
    }

    // 8. Décrémenter le compteur de liens de l'inode
    if (inode->links_count > 0) {
        inode->links_count--;
    }

    // 9. Si plus de liens, libérer les ressources
    if (inode->links_count == 0) {
        // Libérer les blocs de données
        for (int i = 0; i < 12; i++) {
            if (inode->blocks[i] != 0) {
                free_block(fs, inode->blocks[i]);
                inode->blocks[i] = 0;
            }
        }

        // Libérer le bloc indirect si existant
        if (inode->indirect_block != 0) {
            // Libérer d'abord les blocs pointés
            uint32_t *indirect_blocks = (uint32_t *)fs->partition.blocks[inode->indirect_block].data;
            for (uint32_t i = 0; i < fs->superblock.block_size / sizeof(uint32_t); i++) {
                if (indirect_blocks[i] != 0) {
                    free_block(fs, indirect_blocks[i]);
                }
            }
            // Puis libérer le bloc indirect lui-même
            free_block(fs, inode->indirect_block);
            inode->indirect_block = 0;
        }

        // Réinitialiser l'inode
        inode->size = 0;
        inode->is_used = false;
        inode->modified_at = time(NULL);
    }

    // 10. Mettre à jour les métadonnées du parent
    parent->modified_at = time(NULL);

    return 0;
}
