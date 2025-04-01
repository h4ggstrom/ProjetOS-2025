/**
This file contains the functions that are used to read and write to files.

@author Robin de Angelis (%)
@author Alexandre Ledard (%)
@author Killian Treuil (%)

@version 0.0 18/03/2025
*/

#include "file_operations.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> // Pour parcourir les répertoires

/**
 * Create a file with optional content.
 * 
 * @param path The path of the file to create.
 * @param content The content to write to the file (can be NULL).
 * @return 0 on success, -1 on failure.
 */
int create_file(const char *path, const char *content) {
    FILE *file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "Error creating file: %s\n", strerror(errno));
        return -1;
    }
    if (content) {
        fprintf(file, "%s", content);
    }
    fclose(file);
    printf("File created successfully: %s\n", path);
    return 0;
}

/**
 * Delete a file.
 * 
 * @param path The path of the file to delete.
 * @return 0 on success, -1 on failure.
 */
int delete_file(const char *path) {
    if (unlink(path) == -1) {
        fprintf(stderr, "Error deleting file: %s\n", strerror(errno));
        return -1;
    }
    printf("File deleted successfully: %s\n", path);
    return 0;
}

int mycp(const char *source_path, const char *dest_path) {
    int source_fd, dest_fd;
    char buffer[4096];
    ssize_t bytes_read, bytes_written;

    // Open the source file
    source_fd = open(source_path, O_RDONLY);
    if (source_fd < 0) {
        perror("Error opening source file");
        return -1;
    }

    // If destination path is not provided, use the current directory
    char default_dest[256];
    if (!dest_path) {
        const char *filename = strrchr(source_path, '/');
        if (filename) {
            filename++; // Skip the '/'
        } else {
            filename = source_path;
        }
        snprintf(default_dest, sizeof(default_dest), "./%s", filename);
        dest_path = default_dest;
    }

    // Open the destination file
    dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("Error opening destination file");
        close(source_fd);
        return -2;
    }

    // Copy data from source to destination
    while ((bytes_read = read(source_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(source_fd);
            close(dest_fd);
            return -3;
        }
    }

    if (bytes_read < 0) {
        perror("Error reading from source file");
        close(source_fd);
        close(dest_fd);
        return -4;
    }

    // Close file descriptors
    close(source_fd);
    close(dest_fd);

    return 0; // Success
}

/**
 * @brief function to move a file from one location to another.
 * 
 * @param source_path source file path
 * @param dest_dir destination directory path
 * @return int
 * @retval 0 on success
 * @retval -1 on failure
 */
int mymv(const char *source_path, const char *dest_dir) {
    char dest_path[4096];
    const char *filename = strrchr(source_path, '/');
    
    // Extract the filename from the source path
    if (filename) {
        filename++; // Skip the '/'
    } else {
        filename = source_path;
    }

    // Construct the destination path
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, filename);

    // Use rename to move the file
    if (rename(source_path, dest_path) < 0) {
        perror("Error moving file");
        return -1;
    }

    return 0; // Success
}

/**
 * Print the content of a directory.
 * 
 * @param path The path of the directory to print. If NULL or empty, the current directory is used.
 * @return 0 on success, -1 on failure.
 */
int print_directory_content(const char *path) {
    DIR *dir;
    struct dirent *entry;

    // Si aucun chemin n'est fourni, utiliser le répertoire courant
    if (path == NULL || strlen(path) == 0) {
        path = ".";
    }

    dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "Error opening directory '%s': %s\n", path, strerror(errno));
        return -1;
    }

    printf("Contenu du répertoire '%s' :\n", path);
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}