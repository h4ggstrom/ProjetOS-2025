/**
This file contains the tests for the links.c file.

@see links.c

@author Robin de Angelis (%)
@author Alexandre Ledard (%)
@author Killian Treuil (%)

@version 0.0 18/03/2025
*/

#include <criterion/criterion.h>
#include <criterion/logging.h>
#include "../include/links.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/limits.h>

Test(create_hard_link, success) {
    const char *target = "test_target.txt";
    const char *linkpath = "test_hard_link.txt";

    // Create a test target file
    FILE *file = fopen(target, "w");
    cr_assert_not_null(file, "Failed to create test target file.");
    fprintf(file, "Test content");
    fclose(file);

    // Test creating a hard link
    int result = create_hard_link(target, linkpath);
    cr_assert_eq(result, 0, "create_hard_link failed with result: %d", result);

    // Verify the hard link
    struct stat target_stat, link_stat;
    cr_assert_eq(stat(target, &target_stat), 0, "Failed to stat target file.");
    cr_assert_eq(stat(linkpath, &link_stat), 0, "Failed to stat hard link.");
    cr_assert_eq(target_stat.st_ino, link_stat.st_ino, "Hard link inode does not match target inode.");

    // Cleanup
    remove(target);
    remove(linkpath);
}

Test(create_soft_link, success) {
    const char *target = "test_target.txt";
    const char *linkpath = "test_soft_link.txt";

    // Create a test target file
    FILE *file = fopen(target, "w");
    cr_assert_not_null(file, "Failed to create test target file.");
    fprintf(file, "Test content");
    fclose(file);

    // Test creating a symbolic link
    int result = create_soft_link(target, linkpath);
    cr_assert_eq(result, 0, "create_soft_link failed with result: %d", result);

    // Verify the symbolic link
    char buffer[PATH_MAX];
    ssize_t len = readlink(linkpath, buffer, sizeof(buffer) - 1);
    cr_assert_neq(len, -1, "Failed to read symbolic link.");
    buffer[len] = '\0';
    cr_assert_str_eq(buffer, target, "Symbolic link does not point to the correct target.");

    // Cleanup
    remove(target);
    remove(linkpath);
}

Test(display_soft_link_target, success) {
    const char *target = "test_target.txt";
    const char *linkpath = "test_soft_link.txt";

    // Create a test target file
    FILE *file = fopen(target, "w");
    cr_assert_not_null(file, "Failed to create test target file.");
    fprintf(file, "Test content");
    fclose(file);

    // Create a symbolic link
    int result = create_soft_link(target, linkpath);
    cr_assert_eq(result, 0, "create_soft_link failed with result: %d", result);

    // Test displaying the symbolic link target
    result = display_soft_link_target(linkpath);
    cr_assert_eq(result, 0, "display_soft_link_target failed with result: %d", result);

    // Cleanup
    remove(target);
    remove(linkpath);
}

Test(create_hard_link, failure_invalid_target) {
    const char *invalid_target = "non_existent.txt";
    const char *linkpath = "test_hard_link.txt";

    // Test creating a hard link with an invalid target
    int result = create_hard_link(invalid_target, linkpath);
    cr_assert_eq(result, -1, "create_hard_link should have failed for an invalid target.");
}

Test(create_soft_link, failure_invalid_target) {
    const char *invalid_target = "non_existent.txt";
    const char *linkpath = "test_soft_link.txt";

    // Test creating a symbolic link with an invalid target
    int result = create_soft_link(invalid_target, linkpath);
    cr_assert_eq(result, -1, "create_soft_link should have failed for an invalid target.");
}

Test(display_soft_link_target, failure_invalid_link) {
    const char *invalid_linkpath = "non_existent_link.txt";

    // Test displaying the target of a non-existent symbolic link
    int result = display_soft_link_target(invalid_linkpath);
    cr_assert_eq(result, -1, "display_soft_link_target should have failed for a non-existent link.");
}