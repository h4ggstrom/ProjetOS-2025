/**
Main program for creating the required directory structure and files.

This program uses the functions defined in the project to create
directories, files, and links as specified in the requirements.
The structure is created inside a "demo" directory at the root level of the project.

@author 
Robin de Angelis (%)
Alexandre Ledard (%)
Killian Treuil (%)

@version 0.1 01/04/2025
*/

#include <stdio.h>
#include "../include/partition.h"
#include "../include/file_operations.h"
#include "../include/links.h"

int main() {
    // Création des répertoires
    create_directories_recursively("demo/rep01/rep01_01");
    create_directories_recursively("demo/rep01/rep01_02");
    create_directories_recursively("demo/rep01/rep01_03");
    create_directories_recursively("demo/rep02/rep02_01/rep02_01_01/rep02_01_01_01");

    // Création des fichiers
    create_file("demo/rep01/file_test.txt", "Contenu de file_test.txt");
    create_file("demo/rep01/rep01_01/file_test.txt", "Contenu de file_test.txt");
    create_file("demo/rep01/rep01_02/file_test_link.txt", "Contenu de file_test_link.txt");
    create_file("demo/rep01/rep01_03/file_test.txt", "Contenu de file_test.txt");
    create_file("demo/rep02/file_test_lien_dur.txt", "Contenu de file_test_lien_dur.txt");
    create_file("demo/rep02/rep02_01/file_test.txt", "Contenu de file_test.txt");
    create_file("demo/rep02/rep02_01/rep02_01_01/rep02_01_01_01/file_test.txt", "Contenu de file_test.txt");

    // Création des liens symboliques et durs
    create_soft_link("demo/rep01/rep01_02/file_test_link.txt", "demo/rep01/file_test_lien_symbolique.txt");
    create_hard_link("demo/rep02/file_test_lien_dur.txt", "demo/rep02/rep02_01/file_test_lien_dur.txt");

    printf("Arborescence créée avec succès dans le dossier 'demo' !\n");
    return 0;
}