#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

void execute_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: argument attendu\n");
    } else {
        // Vérifier si le répertoire existe (optionnel)
        DIR* dir = opendir(args[1]);
        if (dir) {
            closedir(dir);
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        } else {
            fprintf(stderr, "cd: %s: Répertoire inexistant\n", args[1]);
        }
    }
}
/*
int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    char *token;
    int status;
    pid_t pid;

    while (1) {
        // Afficher le prompt
        printf("myshell> ");
        fflush(stdout);

        // Lire la ligne de commande
        if (!fgets(line, MAX_LINE, stdin)) {
            break; // Sortir si EOF (Ctrl+D)
        }

        // Supprimer le saut de ligne
        line[strcspn(line, "\n")] = '\0';

        // Tokenizer la ligne pour obtenir les arguments
        int i = 0;
        token = strtok(line, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Terminer le tableau avec NULL

        // Gérer les commandes internes
        if (strcmp(args[0], "exit") == 0) {
            break;
        } else if (strcmp(args[0], "cd") == 0) {
            execute_cd(args);
            continue; // Passer à l'itération suivante sans forker
        }
        // Forker un processus enfant
        pid = fork();
        if (pid == 0) {
            // Processus enfant
            execvp(args[0], args);
            // Si execvp retourne, c'est qu'il y a eu une erreur
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Erreur lors du fork
            perror("fork");
        } else {
            // Processus parent - attendre la fin de l'enfant
            waitpid(pid, &status, 0);
        }
    }

    return EXIT_SUCCESS;
}*/