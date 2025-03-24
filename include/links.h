#ifndef LINKS_H
#define LINKS_H

#include <stddef.h> // Pour size_t

/**
 * Create a hard link between two files.
 * 
 * @param target The target file to link to.
 * @param linkpath The path of the new hard link.
 * @return 0 on success, -1 on failure.
 */
int create_hard_link(const char *target, const char *linkpath);

/**
 * Create a symbolic (soft) link between two files.
 * 
 * @param target The target file to link to.
 * @param linkpath The path of the new symbolic link.
 * @return 0 on success, -1 on failure.
 */
int create_soft_link(const char *target, const char *linkpath);

/**
 * Display the target of a symbolic link.
 * 
 * @param linkpath The path of the symbolic link.
 * @return 0 on success, -1 on failure.
 */
int display_soft_link_target(const char *linkpath);

#endif // LINKS_H