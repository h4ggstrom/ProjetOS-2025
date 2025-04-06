# ProjetOS-2025

[FR] Ce projet est un gestionnaire simple de fichier, développé en C dans le cadre de l'UE Systèmes d'exploitation.

[EN] This project is a simple file manager, programmed in C for pour operating system class.

## Installation

### Prérequis | Requirements

- Un compilateur C (`gcc`) | A C compiler (`gcc`)
- Un `make` installé | A `make` installed
- Un système Unix/Linux (ou WSL sur windows) | A Unix/Linux system (or WSL on windows)

### Etapes

1. Cloner le dépôt (ou télécharger le projet) | Clone the github repository (or download the project):

```bash
> git clone https://github.com/h4ggstrom/ProjetOS-2025
> cd ProjectOS-2025
```

2. Compiler le projet avec `make` | Compile the project with `make` :

```bash
> make
```

Le fichier exécutable est généré dans /bin | The executable file is generated in /bin.

3. Executer le programme :

```bash
> ./bin/main
```

### Commandes disponibles | Available commands

| Commande / Command | Syntaxe / Syntax                                  | Description                                                                             |
| ------------------ | ------------------------------------------------- | --------------------------------------------------------------------------------------- |
| help               | help                                              | Affiche les commandes disponibles / Show the available commands.                        |
| exit               | exit                                              | Quitter le terminal / Quit the terminal.                                                |
| clear              | clear                                             | Vider le terminal / Clear the terminal.                                                 |
| cd                 | cd \<dir_name>                                    | Changer de répertoire / Change directory.                                               |
| ls                 | ls \<dir_name> -l (optionnal)                     | Liste le contenu d'un répertoire / List directory files. '-l' for permissions too.      |
| make_demo          | make_demo                                         | Créée les fichiers pour la démonstration / Creates files for the demonstration.         |
| build              | build                                             | Créée la partition / Build the partition.                                               |
| load               | load                                              | Charge la partition / Load the partition.                                               |
| getcwd             | getcwd                                            | Donne la position actuelle dans le répetoire / Give the actual position.                |
| tree               | tree \<dir_name>                                  | Créée l'arborescense des fichiers / Create the file tree.                               |
| create_file        | create_file \<file_name>                          | Créée un fichier / Create a file.                                                       |
| remove_file        | remove_file \<file_name>                          | Supprime un fichier / Delete a file.                                                    |
| open_file          | open_file \<file_name>                            | Ouvrir un fichier / Open a file.                                                        |
| close_file         | close_file \<file_name>                           | Fermer un fichier / Close a file.                                                       |
| create_directory   | create_directory \<dir_name>                      | Créée un répertoire / Create a directory.                                               |
| remove_directory   | remove_directory \<dir_name>                      | Supprime un répertoire / Delete a directory.                                            |
| load               | load                                              | Chargement de la partition / Load the partition.                                        |
| chdir              | chdir \<dir_name>                                 | Changer de répertoire / Change directory.                                               |
| add_user           | add_user \<username> \<groupid> \<user_type>      | Ajouter un nouvel utilisateur / Add a new user. User_types : "admin", "user", "guest".  |
| remove_user        | remove_user \<user_id>                            | Supprimer un utilisateur / Delete a user.                                               |
| list_users         | list_users                                        | Lister tout les utilisateurs / List all users.                                          |
| chmod              | chmod \<path> \<permissions>                      | Modifier les droits d'accès d'un fichier / Modify file access.                          |
| check_permissions  | check_permissions \<path> \<required_permissions> | Vérifier une permission pour un fichier / Check a permission for a file.                |
| chown              | chown \<path> \<new_owner_id> \<new_group_id>     | Changer le propriétaire et le groupe d'un fichier / Change owner and group of a file.   |
| switch_user        | switch_user \<user_id>                            | Changer d'utilisateur / Switch user.                                                    |

### Structure du projet

```
ProjetOS-2025/
├── src/                  # Fichiers d'en-tête / Header files
|   ├── config.h
|   ├── constantes.g
|   ├── file_operations.h
│   ├── links.h
│   ├── partition.h
|   ├── permissions.h
|   ├── tree.h
|   ├── user.h
|   ├── utils.h
│   └── vfs_function.h
├── src/                  # Fichiers source C / C source file
|   ├── file_operations.c
|   ├── main.c
|   ├── partition.c
│   ├── permissions.c
│   ├── tree.c
|   ├── user.c
|   ├── utils.c
│   └── vfs_function.c
├── tests/               # Fichiers tests / tests files
│   ├── test_file_operations.c
|   ├── test_hello_world.c
|   ├── test_links.c
|   ├── test_partition.c
|   ├── test_permissions.c
│   └── test_utils.c
├── LICENSE
├── Makefile
└── README.md
```

### Auteurs

- Alexandre Ledard
- Robin de Angelis
- Killian Treuil
