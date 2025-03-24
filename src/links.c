/**
This file contains the functions to manage the links between the different files.

@author Robin de Angelis (%)
@author Alexandre Ledard (%)
@author Killian Treuil (%)

@version 0.0 18/03/2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h> // Ajouté pour PATH_MAX
#include <linux/limits.h>

/**
 * Create a hard link between two files.
 * 
 * @param target The target file to link to.
 * @param linkpath The path of the new hard link.
 * @return 0 on success, -1 on failure.
 */
int create_hard_link(const char *target, const char *linkpath) {
    if (link(target, linkpath) == -1) {
        fprintf(stderr, "Error creating hard link: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/**
 * Create a symbolic (soft) link between two files.
 * 
 * @param target The target file to link to.
 * @param linkpath The path of the new symbolic link.
 * @return 0 on success, -1 on failure.
 */
int create_soft_link(const char *target, const char *linkpath) {
    if (symlink(target, linkpath) == -1) {
        fprintf(stderr, "Error creating symbolic link: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/**
 * Display the target of a symbolic link.
 * 
 * @param linkpath The path of the symbolic link.
 * @return 0 on success, -1 on failure.
 */
int display_soft_link_target(const char *linkpath) {
    char buffer[PATH_MAX]; // PATH_MAX est maintenant défini
    ssize_t len = readlink(linkpath, buffer, sizeof(buffer) - 1);
    if (len == -1) {
        fprintf(stderr, "Error reading symbolic link: %s\n", strerror(errno));
        return -1;
    }
    buffer[len] = '\0';
    printf("Symbolic link '%s' points to '%s'\n", linkpath, buffer);
    return 0;
}