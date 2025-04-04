#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <asm-generic/fcntl.h>

#define MAX_FILES 100
#define MAX_NAME_LENGTH 256
#define MAX_OPEN_FILES 20
#define SUCCESS 0
#define ERR_FILE_NOT_FOUND -1
#define ERR_NO_SPACE_LEFT -2
#define ERR_PERMISSION_DENIED -3
#define ERR_INVALID_FD -4
#define ERR_EOF -5

typedef struct {
    uint32_t first_block;   // Premier bloc du fichier
    uint32_t size;          // Taille en octets
    char name[MAX_NAME_LENGTH]; // Nom du fichier
    bool is_used ;           // Si l'entrée est utilisée
} FileEntry;

typedef struct {
    int fd;                 // Descripteur de fichier
    uint32_t file_index;    // Index dans la table des fichiers
    uint32_t position;      // Position courante dans le fichier
    int flags;              // Flags d'ouverture
} FileDescriptor;

typedef struct {
    FileEntry files[MAX_FILES];
    FileDescriptor open_files[MAX_OPEN_FILES];
    uint32_t next_fd;       // Prochain FD disponible
} VirtualFS;

int vfs_open(VirtualFS *vfs, const char *pathname, int flags) {
    // 1. Trouver le fichier dans la table
    int file_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (vfs->files[i].is_used && strcmp(vfs->files[i].name, pathname) == 0) {
            file_index = i;
            break;
        }
    }

    // 2. Gestion des différents flags
    if (flags & O_CREAT) {
        if (file_index == -1) {
            // Créer un nouveau fichier
            for (int i = 0; i < MAX_FILES; i++) {
                if (!vfs->files[i].is_used) {
                    file_index = i;
                    strncpy(vfs->files[i].name, pathname, MAX_NAME_LENGTH);
                    vfs->files[i].first_block = 0xFFFFFFFF; // Aucun bloc alloué
                    vfs->files[i].size = 0;
                    vfs->files[i].is_used = true;
                    break;
                }
            }
            if (file_index == -1) {
                return -1; // Plus de place pour de nouveaux fichiers
            }
        }
    } else {
        if (file_index == -1) {
            return -1; // Fichier non trouvé
        }
    }

    // 3. Vérifier les permissions (simplifié)
    if ((flags & O_RDONLY) && !(vfs->files[file_index].is_used)) {
        return -1; // Permission denied
    }

    // 4. Trouver un slot libre dans la table des fichiers ouverts
    int fd = -1;
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (vfs->open_files[i].fd == -1) {
            fd = vfs->next_fd++;
            vfs->open_files[i].fd = fd;
            vfs->open_files[i].file_index = file_index;
            vfs->open_files[i].position = 0;
            vfs->open_files[i].flags = flags;
            break;
        }
    }

    return fd;
}

void vfs_init(VirtualFS *vfs) {
    // Initialiser la table des fichiers
    for (int i = 0; i < MAX_FILES; i++) {
        vfs->files[i].is_used = false;
    }

    // Initialiser la table des fichiers ouverts
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        vfs->open_files[i].fd = -1;
    }

    vfs->next_fd = 0;
}

FileDescriptor *get_file_descriptor(VirtualFS *vfs, int fd) {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (vfs->open_files[i].fd == fd) {
            return &vfs->open_files[i];
        }
    }
    return NULL;
}

int vfs_close(VirtualFS *vfs, int fd) {
    // Vérifications des paramètres
    if (vfs == NULL) {
        perror("Système de fichiers invalide (NULL)");
        return ERR_INVALID_FD;
    }

    if (fd < 0) {
        perror("Descripteur de fichier invalide (négatif)");
        return ERR_INVALID_FD;
    }

    // Recherche du descripteur
    FileDescriptor *desc = NULL;
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (vfs->open_files[i].fd == fd) {
            desc = &vfs->open_files[i];
            break;
        }
    }

    if (desc == NULL) {
        perror("Descripteur de fichier non trouvé");
        return ERR_INVALID_FD;
    }

    // Si le fichier était ouvert en écriture, on pourrait vouloir
    // flusher les buffers ici (dans une implémentation plus avancée)

    // Réinitialisation de l'entrée
    desc->fd = -1;
    desc->file_index = 0;
    desc->position = 0;
    desc->flags = 0;

    return SUCCESS;
}

