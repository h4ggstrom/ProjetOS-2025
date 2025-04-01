#ifndef PARTITION_H
#define PARTITION_H

#include <sys/types.h>

/**
 * Create an empty partition (directory).
 * 
 * @param path The path of the directory to create.
 * @return 0 on success, -1 on failure.
 */
int create_partition(const char *path);

/**
 * Create and mount a virtual partition.
 * 
 * @param file_path The path of the file to use as a partition.
 * @param mount_point The directory where the partition will be mounted.
 * @param size_mb The size of the partition in megabytes.
 * @return 0 on success, -1 on failure.
 */
int create_virtual_partition(const char *file_path, const char *mount_point, int size_mb);

/**
 * Create a directory.
 * 
 * @param path The path of the directory to create.
 * @return 0 on success, -1 on failure.
 */
int create_directory(const char *path);

/**
 * Create a directory with specific permissions.
 * 
 * @param path The path of the directory to create.
 * @param permissions The permissions to set for the directory.
 * @return 0 on success, -1 on failure.
 */
int create_directory_with_permissions(const char *path, mode_t permissions);

/**
 * Create directories recursively.
 * 
 * @param path The path of the directories to create.
 * @return 0 on success, -1 on failure.
 */
int create_directories_recursively(const char *path);

#endif // PARTITION_H