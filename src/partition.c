/**
This file contains the functions to create and manage partitions.

@author Robin de Angelis (%)
@author Alexandre Ledard (%)
@author Killian Treuil (%)

@version 0.1 01/04/2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

/**
 * Create an empty partition (directory).
 * 
 * @param path The path of the directory to create.
 * @return 0 on success, -1 on failure.
 */
int create_partition(const char *path) {
    if (mkdir(path, 0755) == -1) {
        if (errno == EEXIST) {
            fprintf(stderr, "Error: Directory already exists: %s\n", path);
        } else {
            fprintf(stderr, "Error creating directory: %s\n", strerror(errno));
        }
        return -1;
    }
    printf("Partition created successfully: %s\n", path);
    return 0;
}

/**
 * Create a directory.
 * 
 * @param path The path of the directory to create.
 * @return 0 on success, -1 on failure.
 */
int create_directory(const char *path) {
    if (mkdir(path, 0755) == -1) {
        if (errno == EEXIST) {
            fprintf(stderr, "Error: Directory already exists: %s\n", path);
        } else {
            fprintf(stderr, "Error creating directory: %s\n", strerror(errno));
        }
        return -1;
    }
    printf("Directory created successfully: %s\n", path);
    return 0;
}

/**
 * Create a directory with specific permissions.
 * 
 * @param path The path of the directory to create.
 * @param permissions The permissions to set for the directory.
 * @return 0 on success, -1 on failure.
 */
int create_directory_with_permissions(const char *path, mode_t permissions) {
    if (mkdir(path, permissions) == -1) {
        if (errno == EEXIST) {
            fprintf(stderr, "Error: Directory already exists: %s\n", path);
        } else {
            fprintf(stderr, "Error creating directory: %s\n", strerror(errno));
        }
        return -1;
    }
    printf("Directory created successfully: %s\n", path);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h>

/**
 * Create and mount a virtual partition.
 * 
 * @param file_path The path of the file to use as a partition.
 * @param mount_point The directory where the partition will be mounted.
 * @param size_mb The size of the partition in megabytes.
 * @return 0 on success, -1 on failure.
 */
int create_virtual_partition(const char *file_path, const char *mount_point, int size_mb) {
    char command[256];

    // Step 1: Create a file of the specified size
    snprintf(command, sizeof(command), "dd if=/dev/zero of=%s bs=1M count=%d", file_path, size_mb);
    if (system(command) != 0) {
        fprintf(stderr, "Error creating partition file: %s\n", strerror(errno));
        return -1;
    }

    // Step 2: Format the file with ext4
    snprintf(command, sizeof(command), "mkfs.ext4 %s", file_path);
    if (system(command) != 0) {
        fprintf(stderr, "Error formatting partition file: %s\n", strerror(errno));
        return -1;
    }

    // Step 3: Create the mount point directory
    snprintf(command, sizeof(command), "mkdir -p %s", mount_point);
    if (system(command) != 0) {
        fprintf(stderr, "Error creating mount point: %s\n", strerror(errno));
        return -1;
    }

    // Step 4: Mount the file as a partition
    snprintf(command, sizeof(command), "sudo mount %s %s", file_path, mount_point);
    if (system(command) != 0) {
        fprintf(stderr, "Error mounting partition: %s\n", strerror(errno));
        return -1;
    }

    printf("Virtual partition created and mounted successfully at %s\n", mount_point);
    return 0;
}

/**
 * Create directories recursively.
 * 
 * @param path The path of the directories to create.
 * @return 0 on success, -1 on failure.
 */
int create_directories_recursively(const char *path) {
    char temp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    if (temp[len - 1] == '/')
        temp[len - 1] = '\0';

    for (p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
                fprintf(stderr, "Error creating directory '%s': %s\n", temp, strerror(errno));
                return -1;
            }
            *p = '/';
        }
    }

    if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
        fprintf(stderr, "Error creating directory '%s': %s\n", temp, strerror(errno));
        return -1;
    }

    return 0;
}