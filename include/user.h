/**
 * @file user.h
 * @brief Gestion des utilisateurs du système de fichiers
 *
 * @details Ce fichier d'en-tête définit la structure des utilisateurs et les fonctions
 *          pour gérer leur cycle de vie (création, suppression, modification). Il inclut
 *          également les différents types d'utilisateurs et leurs permissions.
 *
 * @version 1.0
 * @date 06/04/2025
 *
 * @authors
 * - Robin de Angelis 100(%)
 * - Alexandre Ledard 0(%)
 * - Killian Treuil 0(%)
 *
 * @copyright Pas de copyright spécifié
 *
 * @defgroup user_management Gestion des utilisateurs
 * @{
 */


#ifndef USER_H
#define USER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct User {
    uint32_t user_id;       // Identifiant unique de l'utilisateur
    uint32_t group_id;      // Identifiant du groupe auquel appartient l'utilisateur
    char username[32];      // Nom d'utilisateur
    uint16_t permissions;   // Permissions par défaut de l'utilisateur (rwxrwxrwx)
} User;

// Types d'utilisateurs
typedef enum {
    USER_TYPE_ADMIN,
    USER_TYPE_USER,
    USER_TYPE_GUEST
} UserType;

// Prototypes des fonctions définies dans user.c

/**
 * @brief Ajoute un nouvel utilisateur.
 * 
 * @param username Nom de l'utilisateur.
 * @param group_id Groupe de l'utilisateur.
 * @param user_type Type de l'utilisateur (admin, user, guest).
 * @return uint32_t L'ID de l'utilisateur ajouté, ou -1 si échec.
 */
uint32_t add_user(const char *username, uint32_t group_id, UserType user_type);

/**
 * @brief Récupère un utilisateur par son ID.
 * 
 * @param user_id ID de l'utilisateur.
 * @return User* Pointeur vers l'utilisateur, ou NULL si non trouvé.
 */
User *get_user(uint32_t user_id);

/**
 * @brief Définit l'utilisateur courant.
 * 
 * @param user_id ID de l'utilisateur à définir comme courant.
 * @return true si l'utilisateur a été changé avec succès, false sinon.
 */
bool set_current_user(uint32_t user_id);

/**
 * @brief Récupère l'utilisateur courant.
 * 
 * @return User* Pointeur vers l'utilisateur courant, ou NULL si aucun utilisateur n'est défini.
 */
User *get_current_user();

/**
 * @brief Supprime un utilisateur par son ID.
 * 
 * @param user_id ID de l'utilisateur à supprimer.
 * @return true si l'utilisateur a été supprimé avec succès, false sinon.
 */
bool remove_user(uint32_t user_id);

/**
 * @brief Récupère le nombre d'utilisateurs enregistrés.
 * 
 * @return uint32_t Le nombre d'utilisateurs.
 */
uint32_t get_user_count();

/**
 * @brief Affiche tous les utilisateurs disponibles.
 */
void display_users();

#endif // USER_H

/** @} */ // Fin du groupe user_management