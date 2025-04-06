/**
 * @file links.h
 * @brief Gestion des liens (hard links et symbolic links) dans le système de fichiers
 *
 * @details Ce fichier d'en-tête définit les fonctions pour créer et gérer les liens
 *          entre fichiers, incluant à la fois les liens physiques (hard links) et
 *          les liens symboliques (soft links).
 *
 * @version 1.0
 * @date 06/04/2025
 *
 * @authors
 * - Robin de Angelis 0 (%)
 * - Alexandre Ledard 100 (%)
 * - Killian Treuil 0 (%)
 *
 * @copyright Pas de copyright spécifié
 *
 * @defgroup link_management Gestion des liens
 * @{
 */

 #ifndef LINKS_H
 #define LINKS_H
 
 #include <partition.h>
 
 /**
  * @brief Crée un lien physique (hard link) vers un fichier existant
  * 
  * @param fs Pointeur vers le système de fichiers
  * @param oldpath Chemin existant du fichier
  * @param newpath Nouveau chemin à créer
  * @return int 0 en cas de succès, -1 en cas d'échec
  */
 int fs_link(FileSystem *fs, const char *oldpath, const char *newpath);
 
 /**
  * @brief Crée un lien physique entre deux fichiers
  * 
  * @param target Fichier cible à lier
  * @param linkpath Chemin du nouveau lien physique
  * @return int 0 si succès, -1 si échec
  */
 int create_hard_link(const char *target, const char *linkpath);
 
 /**
  * @brief Crée un lien symbolique entre deux fichiers
  * 
  * @param target Fichier cible à lier
  * @param linkpath Chemin du nouveau lien symbolique
  * @return int 0 si succès, -1 si échec
  */
 int create_soft_link(const char *target, const char *linkpath);
 
 /**
  * @brief Affiche la cible d'un lien symbolique
  * 
  * @param linkpath Chemin du lien symbolique
  * @return int 0 si succès, -1 si échec
  */
 int display_soft_link_target(const char *linkpath);
 
 #endif // LINKS_H
 
 /** @} */ // Fin du groupe link_management