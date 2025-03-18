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