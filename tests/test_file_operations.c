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
#include "../include/file_operations.h" // Inclure la fonction mycp

// Fonction utilitaire pour créer un fichier temporaire avec du contenu
void create_temp_file(const char *path, const char *content) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    cr_assert(fd >= 0, "Failed to create temporary file: %s", path);
    write(fd, content, strlen(content));
    close(fd);
}

// Test pour vérifier la copie d'un fichier
Test(file_operations, test_mycp_success) {
    const char *source_path = "test_source.txt";
    const char *dest_path = "test_dest.txt";
    const char *content = "Hello, world!";

    // Créer un fichier source temporaire
    create_temp_file(source_path, content);

    // Appeler la fonction mycp
    int result = mycp(source_path, dest_path);

    // Vérifier que la fonction a réussi
    cr_assert_eq(result, 0, "mycp failed with result: %d", result);

    // Vérifier que le fichier de destination contient les mêmes données
    char buffer[256] = {0};
    int fd = open(dest_path, O_RDONLY);
    cr_assert(fd >= 0, "Failed to open destination file: %s", dest_path);
    read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    cr_assert_str_eq(buffer, content, "Content mismatch between source and destination");

    // Nettoyer les fichiers temporaires
    remove(source_path);
    remove(dest_path);
}

// Test pour vérifier le comportement avec un fichier source inexistant
Test(file_operations, test_mycp_source_not_found) {
    const char *source_path = "nonexistent.txt";
    const char *dest_path = "test_dest.txt";

    // Appeler la fonction mycp
    int result = mycp(source_path, dest_path);

    // Vérifier que la fonction retourne une erreur
    cr_assert_eq(result, -1, "mycp should fail when source file does not exist");

    // Vérifier que le fichier de destination n'a pas été créé
    cr_assert(access(dest_path, F_OK) == -1, "Destination file should not exist");

    // Nettoyer les fichiers temporaires (au cas où)
    remove(dest_path);
}