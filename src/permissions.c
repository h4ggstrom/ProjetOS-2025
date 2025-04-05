/**
 * @file permission.c
 * @brief  This file contains the functions that are used to manage the permissions and access to the files.
 *
 *
 * @version 0.3
 * @date 04/04/2025
 *
 * @authors
 * - Robin de Angelis (%)
 * - Alexandre Ledard (%)
 * - Killian Treuil (%)
 */

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * Change the permissions of a file or directory.
 * 
 * @param path The path of the file or directory.
 * @param mode The new permissions (e.g., 0644, 0755).
 * @return 0 on success, -1 on failure.
 */
int change_permissions(const char *path, mode_t mode) {
    if (chmod(path, mode) == -1) {
        fprintf(stderr, "Error changing permissions: %s\n", strerror(errno));
        return -1;
    }
    printf("Permissions changed successfully: %s\n", path);
    return 0;
}

/**
 * Check if a file or directory is accessible with the given mode.
 * 
 * @param path The path of the file or directory.
 * @param mode The access mode to check (e.g., R_OK, W_OK, X_OK).
 * @return 0 if accessible, -1 otherwise.
 */
int check_access(const char *path, int mode) {
    if (access(path, mode) == -1) {
        fprintf(stderr, "Access check failed for %s: %s\n", path, strerror(errno));
        return -1;
    }
    printf("Access check passed for %s\n", path);
    return 0;
}