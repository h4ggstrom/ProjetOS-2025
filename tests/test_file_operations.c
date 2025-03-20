/**
This file contains the tests for the file_operations.c file.

@see file_operations.c

@author Robin de Angelis (%)
@author Alexandre Ledard (%)
@author Killian Treuil (%)

@version 0.0 18/03/2025
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../include/file_operations.h" // Include the mycp function

// Utility function to create a temporary file with content
static void create_temp_file(const char *path, const char *content) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    cr_assert(fd >= 0, "Failed to create temporary file: %s", path);
    write(fd, content, strlen(content));
    close(fd);
}

// Test to verify successful file copy
Test(file_operations, test_mycp_success) {
    const char *source_path = "test_source.txt";
    const char *dest_path = "test_dest.txt";
    const char *content = "Hello, world!";

    // Create a temporary source file
    create_temp_file(source_path, content);

    // Call the mycp function
    int result = mycp(source_path, dest_path);

    // Verify that the function succeeded
    cr_assert_eq(result, 0, "mycp failed with result: %d", result);

    // Verify that the destination file contains the same data
    char buffer[256] = {0};
    int fd = open(dest_path, O_RDONLY);
    cr_assert(fd >= 0, "Failed to open destination file: %s", dest_path);
    read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    cr_assert_str_eq(buffer, content, "Content mismatch between source and destination");

    // Clean up temporary files
    remove(source_path);
    remove(dest_path);
}  
// Test to verify behavior when source file does not exist
Test(file_operations, test_mycp_source_not_found) {
    const char *source_path = "nonexistent.txt";
    const char *dest_path = "test_dest.txt";

    // Call the mycp function
    int result = mycp(source_path, dest_path);

    // Verify that the function returns an error
    cr_assert_eq(result, -1, "mycp should fail when source file does not exist");

    // Verify that the destination file was not created
    cr_assert(access(dest_path, F_OK) == -1, "Destination file should not exist");

    // Clean up temporary files (just in case)
    remove(dest_path);
}  