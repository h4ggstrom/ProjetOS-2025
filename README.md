# ProjetOS-2025

[FR] Ce projet est un gestionnaire simple de fichier, développé en C dans le cadre de l'UE Systèmes d'exploitation.

[EN] This project is a simple file manager, programmed in C for our operating system class.

## Installation

### Prérequis | Requirements

- Un compilateur C (`gcc`) | A C compiler (`gcc`)
- Un `make` installé | A `make` installed
- Un système Unix/Linux (ou WSL sur windows) | A Unix/Linux system (or WSL on windows)

### Etapes | Steps

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

3. Executer le programme | Execute the program :

```bash
> ./bin/main
```

### Commandes disponibles | Available commands

| Commande / Command | Syntaxe / Syntax                                   | Description                                                                                                      |
| ------------------ | -------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------- |
| help               | help                                               | Affiche les commandes disponibles / Show the available commands.                                                 |
| exit               | exit                                               | Quitter le terminal / Quit the terminal.                                                                         |
| clear              | clear                                              | Vider le terminal / Clear the terminal.                                                                          |
| ls                 | ls \<dir_name> -l (optionnal)                      | Liste le contenu d'un répertoire / List directory files. Add '-l' for permissions too.                           |
| make_demo          | make_demo                                          | Créée les fichiers pour la démonstration / Creates files for the demonstration.                                  |
| build              | build                                              | Créée la partition / Build the partition.                                                                        |
| load               | load                                               | Charge la partition / Load the partition.                                                                        |
| getcwd             | getcwd                                             | Donne la position actuelle dans le répetoire / Give the actual position.                                         |
| tree               | tree \<dir_name>                                   | Affiche l'arborescense du répertoire / Show the directory tree.                                                  |
| create_file        | create_file \<file_name>                           | Créée un fichier / Create a file.                                                                                |
| remove_file        | remove_file \<file_name>                           | Supprime un fichier / Delete a file.                                                                             |
| open_file          | open_file \<file_name>                             | Ouvrir un fichier / Open a file.                                                                                 |
| close_file         | close_file \<file_name>                            | Fermer un fichier / Close a file.                                                                                |
| write              | write \<descripteur> \<message>                    | Ecrire un message dans un fichier ouvert / Write a message in a opened file.                                     |
| read               | read \<descripteur>                                | Lire le contenu d'un fichier ouvert / Read the content of a opened file.                                         |
| lseek              | lseek \<descripteur> \<décalage> \<positionnement> | Permet de se positionner dans un fichier / To position in a file. Positionnement : SEEK_SET, SEEk_CUR, SEEk_END. |
| create_directory   | create_directory \<dir_name>                       | Créée un répertoire / Create a directory.                                                                        |
| remove_directory   | remove_directory \<dir_name>                       | Supprime un répertoire / Delete a directory.                                                                     |
| chdir              | chdir \<dir_name>                                  | Changer de répertoire / Change directory.                                                                        |
| add_user           | add_user \<username> \<groupid> \<user_type>       | Ajouter un nouvel utilisateur / Add a new user. User_types : "admin", "user", "guest".                           |
| remove_user        | remove_user \<user_id>                             | Supprimer un utilisateur / Delete a user.                                                                        |
| list_users         | list_users                                         | Lister tout les utilisateurs / List all users.                                                                   |
| chmod              | chmod \<path> \<permissions>                       | Modifier les droits d'accès d'un fichier / Modify file access.                                                   |
| check_permissions  | check_permissions \<path> \<required_permissions>  | Vérifier une permission pour un fichier / Check a permission for a file.                                         |
| chown              | chown \<path> \<new_owner_id> \<new_group_id>      | Changer le propriétaire et le groupe d'un fichier / Change owner and group of a file.                            |
| switch_user        | switch_user \<user_id>                             | Changer d'utilisateur / Switch user.                                                                             |
| link               | link <source_file> <destination_file>              | Créée un hardlink entre deux fichiers / Creates a hard link between two files.                                   |
| symlink            | symlink <source_file> <destination_file>           | Créée un lien symbolique entre deux fichiers / Creates a soft link between two files.                            |
| read_symlink       | read_symlink <file_name>                           | Renvoie où le fichier renseigné pointe / Gives where the given file is linked.                                   |

### Structure du projet | Project structure

```
ProjetOS-2025/
├── src/                  # Fichiers d'en-tête / Header files
|   ├── constantes.h
│   ├── links.h
│   ├── partition.h
|   ├── permissions.h
|   ├── tree.h
|   ├── user.h
│   └── vfs_function.h
├── src/                  # Fichiers source C / C source file
|   ├── main.c
|   ├── partition.c
│   ├── permissions.c
│   ├── tree.c
|   ├── user.c
│   └── vfs_function.c
├── tests/               # Fichiers tests / Tests files
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

### Auteurs | Authors

- Alexandre Ledard, L3 Informatique <alexandre.ledard@etu.cyu.fr>
- Robin de Angelis, L3 Informatique <robin.de-angelis@etu.cyu.fr>
- Killian Treuil, L3 Informatique <killian.treuil@etu.cyu.fr>
