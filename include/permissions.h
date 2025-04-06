#ifndef PERMISSIONS_H
#define PERMISSIONS_H

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include "user.h"
#include "partition.h"

/**
 * @brief Définit les permissions d'un inode.
 * 
 * @param inode L'inode cible.
 * @param permissions Les nouvelles permissions (format UNIX : rwxrwxrwx).
 * @return true si les permissions ont été définies avec succès, false sinon.
 */
bool set_permissions(Inode *inode, uint16_t permissions);

/**
 * @brief Vérifie si un inode a les permissions nécessaires pour un utilisateur donné.
 * 
 * @param inode L'inode cible.
 * @param required_permissions Les permissions requises (format UNIX : rwxrwxrwx).
 * @param user L'utilisateur pour lequel vérifier les permissions.
 * @return true si les permissions sont suffisantes, false sinon.
 */
bool check_permissions(const Inode *inode, uint16_t required_permissions, const User *user);

/**
 * @brief Change le propriétaire et le groupe d'un inode.
 * 
 * @param inode L'inode cible.
 * @param new_owner_id Le nouvel ID du propriétaire.
 * @param new_group_id Le nouvel ID du groupe.
 * @return true si l'opération a réussi, false sinon.
 */
bool chown_inode(Inode *inode, uint32_t new_owner_id, uint32_t new_group_id);

#endif // PERMISSIONS_H