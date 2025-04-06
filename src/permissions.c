/**
 * @file permission.c
 * @brief  This file contains the functions that are used to manage the permissions and access to the files.
 *
 *
 * @version 0.3
 * @date 04/04/2025
 *
 * @authors
 * - Robin de Angelis (%)
 * - Alexandre Ledard (%)
 * - Killian Treuil (%)
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "partition.h"
#include "permissions.h"
#include "user.h"

/**
 * @brief Définit les permissions d'un inode.
 * 
 * @param inode L'inode cible.
 * @param permissions Les nouvelles permissions (format UNIX : rwxrwxrwx).
 * @return true si les permissions ont été définies avec succès, false sinon.
 */
bool set_permissions(Inode *inode, uint16_t permissions) {
    if (!inode) {
        fprintf(stderr, "Erreur : inode invalide.\n");
        return false;
    }
    inode->permissions = permissions;
    return true;
}

/**
 * @brief Vérifie si un inode a les permissions nécessaires pour un utilisateur donné.
 * 
 * @param inode L'inode cible.
 * @param required_permissions Les permissions requises (format UNIX : rwxrwxrwx).
 * @param user L'utilisateur pour lequel vérifier les permissions.
 * @return true si les permissions sont suffisantes, false sinon.
 */
bool check_permissions(const Inode *inode, uint16_t required_permissions, const User *user) {
    if (!inode || !user) {
        fprintf(stderr, "Erreur : inode ou utilisateur invalide.\n");
        return false;
    }

    // Vérifie les permissions du propriétaire
    if (inode->owner_id == user->user_id) {
        return (inode->permissions & (required_permissions << 6)) == (required_permissions << 6);
    }

    // Vérifie les permissions du groupe
    if (inode->group_id == user->group_id) {
        return (inode->permissions & (required_permissions << 3)) == (required_permissions << 3);
    }

    // Vérifie les permissions globales
    return (inode->permissions & required_permissions) == required_permissions;
}

/**
 * @brief Change le propriétaire et le groupe d'un inode.
 * 
 * @param inode L'inode cible.
 * @param new_owner_id Le nouvel ID du propriétaire.
 * @param new_group_id Le nouvel ID du groupe.
 * @return true si l'opération a réussi, false sinon.
 */
bool chown_inode(Inode *inode, uint32_t new_owner_id, uint32_t new_group_id) {
    if (!inode) {
        fprintf(stderr, "Erreur : inode invalide.\n");
        return false;
    }

    // Met à jour le propriétaire et le groupe
    inode->owner_id = new_owner_id;
    inode->group_id = new_group_id;

    return true;
}

