#include "user.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_USERS 100

static User user_table[MAX_USERS];
static uint32_t user_count = 0;
static User *current_user = NULL; // Utilisateur courant

/**
 * @brief Ajoute un nouvel utilisateur.
 * 
 * @param username Nom de l'utilisateur.
 * @param group_id Groupe de l'utilisateur.
 * @param user_type Type de l'utilisateur (admin, user, guest).
 * @return uint32_t L'ID de l'utilisateur ajouté, ou -1 si échec.
 */
uint32_t add_user(const char *username, uint32_t group_id, UserType user_type) {
    if (user_count >= MAX_USERS) {
        return (uint32_t)-1; // Table pleine
    }

    User *new_user = &user_table[user_count];
    new_user->user_id = user_count;
    new_user->group_id = group_id;
    strncpy(new_user->username, username, sizeof(new_user->username) - 1);
    new_user->username[sizeof(new_user->username) - 1] = '\0';

    // Définir les permissions par défaut en fonction du type d'utilisateur
    switch (user_type) {
        case USER_TYPE_ADMIN:
            new_user->permissions = 0777; // Lecture, écriture, exécution pour tous
            break;
        case USER_TYPE_USER:
            new_user->permissions = 0755; // Lecture, écriture pour le propriétaire, lecture/exécution pour les autres
            break;
        case USER_TYPE_GUEST:
            new_user->permissions = 0444; // Lecture seule pour tous
            break;
        default:
            new_user->permissions = 0000; // Aucune permission par défaut
            break;
    }

    return user_count++;
}

/**
 * @brief Récupère un utilisateur par son ID.
 * 
 * @param user_id ID de l'utilisateur.
 * @return User* Pointeur vers l'utilisateur, ou NULL si non trouvé.
 */
User *get_user(uint32_t user_id) {
    if (user_id >= user_count) {
        return NULL;
    }
    return &user_table[user_id];
}

/**
 * @brief Définit l'utilisateur courant.
 * 
 * @param user_id ID de l'utilisateur à définir comme courant.
 * @return true si l'utilisateur a été changé avec succès, false sinon.
 */
bool set_current_user(uint32_t user_id) {
    User *user = get_user(user_id);
    if (!user) {
        fprintf(stderr, "Erreur : utilisateur avec ID %u introuvable.\n", user_id);
        return false;
    }

    current_user = user;
    return true;
}

/**
 * @brief Récupère l'utilisateur courant.
 * 
 * @return User* Pointeur vers l'utilisateur courant, ou NULL si aucun utilisateur n'est défini.
 */
User *get_current_user() {
    return current_user;
}

/**
 * @brief Supprime un utilisateur par son ID.
 * 
 * @param user_id ID de l'utilisateur à supprimer.
 * @return true si l'utilisateur a été supprimé avec succès, false sinon.
 */
bool remove_user(uint32_t user_id) {
    User *current = get_current_user();
    if (!current) {
        fprintf(stderr, "Erreur : aucun utilisateur courant.\n");
        return false;
    }

    User *target = get_user(user_id);
    if (!target) {
        fprintf(stderr, "Erreur : utilisateur avec ID %u introuvable.\n", user_id);
        return false;
    }

    // Vérifie que l'utilisateur courant a au moins le même niveau de permissions
    if (current->permissions < target->permissions) {
        fprintf(stderr, "Erreur : permissions insuffisantes pour supprimer l'utilisateur %u.\n", user_id);
        return false;
    }

    // Supprime l'utilisateur en décalant les entrées dans la table des utilisateurs
    for (uint32_t i = user_id; i < user_count - 1; i++) {
        user_table[i] = user_table[i + 1];
    }
    user_count--;

    // Si l'utilisateur supprimé est l'utilisateur courant, réinitialise `current_user`
    if (current_user == target) {
        current_user = NULL;
    }

    return true;
}

/**
 * @brief Récupère le nombre total d'utilisateurs.
 * 
 * @return uint32_t Le nombre total d'utilisateurs.
 */
uint32_t get_user_count() {
    return user_count;
}

/**
 * @brief Affiche tous les utilisateurs disponibles.
 */
void display_users() {
    if (user_count == 0) {
        printf("Aucun utilisateur enregistré.\n");
        return;
    }

    printf("Liste des utilisateurs disponibles :\n");
    for (uint32_t i = 0; i < user_count; i++) {
        printf("ID: %u, Nom: %s, Groupe: %u, Permissions: %o\n",
               user_table[i].user_id,
               user_table[i].username,
               user_table[i].group_id,
               user_table[i].permissions);
    }
}