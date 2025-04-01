#ifndef PERMISSIONS_H
#define PERMISSIONS_H

#include <sys/types.h>

/**
 * Change the permissions of a file or directory.
 * 
 * @param path The path of the file or directory.
 * @param mode The new permissions (e.g., 0644, 0755).
 * @return 0 on success, -1 on failure.
 */
int change_permissions(const char *path, mode_t mode);

/**
 * Check if a file or directory is accessible with the given mode.
 * 
 * @param path The path of the file or directory.
 * @param mode The access mode to check (e.g., R_OK, W_OK, X_OK).
 * @return 0 if accessible, -1 otherwise.
 */
int check_access(const char *path, int mode);

#endif // PERMISSIONS_H