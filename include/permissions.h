/**
 * @file permissions.h
 * @brief Gestion des permissions et propriétés des fichiers
 *
 * @details Ce fichier d'en-tête définit les fonctions de gestion des permissions
 *          UNIX (rwxrwxrwx) et des propriétaires pour les inodes du système de fichiers.
 *          Il permet de modifier et vérifier les droits d'accès aux fichiers.
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
 * @defgroup permission_management Gestion des permissions
 * @{
 */

 #ifndef PERMISSIONS_H
 #define PERMISSIONS_H
 
 #include <sys/types.h>
 #include <stdint.h>
 #include <stdbool.h>
 #include "user.h"
 #include "partition.h"
 
 /**
  * @brief Définit les permissions d'un inode
  * 
  * @param inode L'inode cible
  * @param permissions Masque de permissions UNIX (rwxrwxrwx)
  * @return true si succès, false en cas d'erreur
  */
 bool set_permissions(Inode *inode, uint16_t permissions);
 
 /**
  * @brief Vérifie les permissions d'accès
  * 
  * @param inode Inode à vérifier
  * @param required_permissions Permissions requises
  * @param user Utilisateur demandant l'accès
  * @return true si l'accès est autorisé, false sinon
  */
 bool check_permissions(const Inode *inode, uint16_t required_permissions, const User *user);
 
 /**
  * @brief Change le propriétaire et groupe d'un inode
  * 
  * @param inode Inode cible
  * @param new_owner_id Nouvel ID propriétaire
  * @param new_group_id Nouvel ID groupe
  * @return true si succès, false en cas d'erreur
  */
 bool chown_inode(Inode *inode, uint32_t new_owner_id, uint32_t new_group_id);
 
 #endif // PERMISSIONS_H
 
 /** @} */ // Fin du groupe permission_management