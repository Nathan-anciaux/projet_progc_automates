/**
 * @file type_hash.h
 * @brief
 * Implémentation des tables de hachage (déjà écrite, à utiliser pour les
 * parcours de graphes).
 *
 * @details
 * Ce fichier contient une implémentation déjà écrite des tables de hachage.
 * Vous ne devez PAS LE MODIFIER. vous pouvez l'utiliser dans votre code pour
 * stocker un ensemble détats déjà visités quand vous faites un parcours de
 * graphe.
 *
 * @remark
 * La table de hachage est similaire à celle fournie dans le projet
 * d'algorithmique. Elle ne peut stocker que des éléments encodés comme des
 * tableaux d'entiers. Cela signifie que pour stocker des objets, vous devrez
 * les encoder sous forme de tableaux d'entiers. De plus, la table de hachage ne
 * stocke pas les éléments eux-mêmes, mais des indices vers un tableau de
 * stockage externe que vous devez gérer vous-même.
 */

#ifndef TYPE_HASH_H
#define TYPE_HASH_H

#include "system_type_basic.h"
#include <stdbool.h>

/**
 * @brief
 * Type utilisé pour représenter une table de hachage.
 *
 * @details
 * Utilisée pour stocker les configurations de plateau visitées dans le
 * résolveur. La table de hachage ne stocke pas les objets eux-mêmes, mais des
 * indices vers un tableau de stockage externe qui est passé aux fonctions de la
 * table de hachage (ceci est nécessaire car le tableau de stockage externe peut
 * avoir besoin d'être réalloué et déplacé en mémoire).
 *
 * @remark
 * Chaque élément inséré doit être encodé dans le tableau de stockage externe
 * comme une séquence d'entiers (la longueur de la séquence est fixée à la
 * création de la table de hachage). En particulier, cela signifie que les
 * objets à insérer doivent être encodés comme des tableaux d'entiers.
 */
typedef struct {
    int hash_capacity; //!< Capacité actuelle de la table de hachage.
    int hash_size;    //!< Taille du hachage (nombre d'éléments dans la table de
                      //!< hachage).
    int *hash_table;  //!< Pointeur vers la table de hachage elle-même (tableau
                      //!< d'indices vers le tableau de stockage externe).
    int element_size; //!< Nombre de valeurs utilisées pour encoder un seul
                      //!< élément dans le tableau de stockage externe.
} hash_table;

/**
 * @brief
 * Crée une nouvelle table de hachage vide.
 *
 * @return
 * La table de hachage vide créée.
 */
hash_table *create_hash_table(
    int element_size //!< Nombre de valeurs utilisées pour encoder un seul
                     //!< élément dans le tableau de stockage externe.
);

/**
 * @brief
 * Supprime une table de hachage.
 */
void delete_hash_table(hash_table *ht //!< La table de hachage à supprimer.
);

/**
 * @brief
 * Tente d'insérer un élément dans la table de hachage.
 *
 * @details
 * Si l'élément est déjà présent dans la table de hachage, l'insertion n'est pas
 * effectuée. De plus l'indice du premier élément identique trouvé est stocké
 * dans la variable pointée par ret (si ret n'est pas NULL).
 *
 * @remark
 * L'indice i fait référence à la position de l'élément dans le tableau de
 * stockage externe. Puisque chaque élément est encodé comme une séquence
 * d'entiers de taille fixe (le champ element_size), la table de hachage va en
 * fait examiner la séquence de element_size valeurs entières commençant à la
 * position i * element_size dans le tableau de stockage externe.
 *
 * @return
 * true si l'élément a été inséré, false s'il était déjà présent. Dans ce
 * dernier cas, la table de hachage n'est pas modifiée.
 */
bool hash_table_insert(
    hash_table *ht, //!< La table de hachage.
    int *elements, //!< Tableau de stockage externe contenant tous les éléments.
    int i,   //!< Indice de l'élément à insérer dans le tableau de stockage
             //!< externe.
    int *ret //!< Pointeur vers un entier où sera stocké l'indice de l'élément
             //!< existant (si déjà présent).
);

#endif
