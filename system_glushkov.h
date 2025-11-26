/**
 * @file glushkov.h
 * @brief
 * Construction d'un NFA à partir d'une expression régulière avec l'algorithme
 * de Glushkov.
 *
 * @attention
 * Cet algorithme est déjà écrit. Vous ne devez PAS LE MODIFIER.
 */

#ifndef REGAUT_H_
#define REGAUT_H_

#include "error.h"
#include "nfa.h"
#include "regexp.h"
#include "type_boolarray.h"
#include <stdlib.h>

/**
 * @brief
 * Compte le nombre de noeuds de type "lettre" dans une expression régulière.
 *
 * @return
 * Le nombre de noeuds de type "lettre" dans l'expression régulière.
 */
int reg_countletters(regexp * //!< Une expression régulière.
);

/**
 * @brief
 * Type utilisé pour représenter l'information que récupère l'algorithme de
 * Glushkov à partir de l'expression régulière (après renommage des lettres)
 * passée en entrée.
 *
 * @remark
 * Cet objet est calculé récursivement à partir de l'expression régulière passée
 * en entrée.
 */
typedef struct {
    int size;     //!< Nombre de lettres dans l'expression régulière après
                  //!< renommage.
    bool epsilon; //!< Booléen indiquant si le langage de l'expression contient
                  //!< le mot vide.
    barray
        *first; //!< Tableau de Booléens indexé par les lettres (taille `size`).
                //!< Pour chaque lettre `a`, indique si il existe un mot dans le
                //!< langage de l'expression qui commence par `a`.
    barray *last; //!< Tableau de Booléens indexé par les lettres (taille
                  //!< `size`). Pour chaque lettre `a`, indique si il  existe un
                  //!< mot dans le langage de l'expression qui termine par `a`.
    barray *follow; //!< Tableau de Booléens indexé par les mots de deux lettres
                    //!< (taille `size * size` : le mot `ab` correspond à la
                    //!< case `a * size + b`). Pour chaque mot `ab`, indique si
                    //!< il existe un mot dans le langage de l'expression qui
                    //!< contient le facteur `ab`.
} glushkov_info;

/**
 * @brief
 * Calcul de l'objet de type glushkov_info. Initialisation sur une
 * sous-expression qui définit un langage vide.
 *
 * @return
 * L'objet de type glushkov_info.
 */
glushkov_info *reg_create_gk_emp(
    int //!<  Nombre de lettres dans l'expression régulière globale.
);

/**
 * @brief
 * Calcul de l'objet de type glushkov_info. Initialisation sur une
 * sous-expression qui définit le singleton contenant le mot vide.
 *
 * @return
 * L'objet de type glushkov_info.
 */
glushkov_info *reg_create_gk_eps(int);

/**
 * @brief
 * Calcul de l'objet de type glushkov_info. Initialisation sur une
 * sous-expression qui définit un singleton contenant un mot d'une lettre.
 *
 * @return
 * L'objet de type glushkov_info.
 */
glushkov_info *reg_create_gk_let(
    int, //!<  Le numéro de la lettre dans le singleton.
    int  //!<  Nombre de lettres dans l'expression régulière globale.
);

/**
 * @brief
 * Calcul de l'objet de type glushkov_info. Cas récursif: étoile de Kleene.
 *
 * @return
 * L'objet de type glushkov_info correspondant à l'étoile de Kleene de celui
 * passé en entrée.
 */
glushkov_info *reg_gk_star(glushkov_info * //!< Un objet de type glushkov_info.
);

/**
 * @brief
 * Calcul de l'objet de type glushkov_info. Cas récursif: union.
 *
 * @return
 * L'objet de type glushkov_info correspondant à l'union des deux qui sont
 * passés en entrée.
 */
glushkov_info *
reg_gk_union(glushkov_info *, //!< Premier objet de type glushkov_info.
             glushkov_info *  //!< Second objet de type glushkov_info.
);

/**
 * @brief
 * Calcul de l'objet de type glushkov_info. Cas récursif: concaténation.
 *
 * @return
 * L'objet de type glushkov_info correspondant à la concaténation des deux qui
 * sont passés en entrée.
 */
glushkov_info *
reg_gk_concat(glushkov_info *, //!< Premier objet de type glushkov_info.
              glushkov_info *  //!< Second objet de type glushkov_info.
);

/**
 * @brief
 * Libération d'un objet de type glushkov_info.
 */
void reg_gk_delete(
    glushkov_info * //!< Un objet de type glushkov_info à libérer.
);

/**
 * @brief
 * Calcul complet de l'objet de type glushkov_info à partir d'une
 * (sous-)expression régulière.
 *
 * @details
 * Le calcul se fait de manière récursive et utilise les fonctions précédentes
 * pour gérer les différents cas. On aura donc besoin du nombre total de noeuds
 * de type lettre contenus dans l'expression complète. Ce nombre est passé en
 * paramètre (il faudra donc l'avoir calculé au préalable).
 *
 * @attention
 * Le "renommage" des lettres contenues de l'expression est fait dynamiquement
 * lors du calcul. On renomme chaque lettre par un entier. Les deux derniers
 * paramètres servent à ce renommage. Le troisième paramètre est un pointeur
 * vers un compteur indiquant le nombre de lettres qu'on a déjà renommées (il
 * est donc incrémenté lorque l'on tombe sur un noeud de type "lettre"). Le
 * quatrième paramètre sert à retourner (par pointeur) la correspondance entre
 * les nouveaux noms et les lettres auxquelles ils correspondent dans
 * l'expression originale.
 *
 *
 * @remark
 * Le calcul est récursif et devra utiliser les fonctions précédentes.
 *
 * @return
 * L'objet de type glushkov_info correspondant à l'expression régulière passé en
 * entrée.
 */
glushkov_info *gk_indexleaves(
    regexp *, //!< Une sous-expression régulière de celle considérée par
              //!< l'algorithme de Glushkov.
    int,      //!< Le nombre total de noeuds de type "lettre" dans l'expression
              //!< complète.
    int *, //!< Un pointeur qui sert à renommer les lettres de l'expression par
           //!< des entiers. Pointe sur le nombre de lettres déjà renommées.
    char * //!< Un tableau qui sert à retourner (par pointeur), les lettres
           //!< auxquelles correspondent les noms dans l'expression originale.
);

/**
 * @brief
 * Algorithme de Glushkov.
 *
 * @details
 * Calcule un NFA à partir d'une expression régulière.
 *
 * @remark
 * L'algorithme de Glushkov ne peut traiter que les expressions régulières
 * simples. Une expression régulière simple n'utilise ni l'intersection ni le
 * complément. On devra vérifier cette propriété au début de l'algorithme et
 * retourner NULL si l'expression passée en entrée n'est pas simple.
 *
 * @return
 * Un NFA qui reconnaît le même langage que l'expression régulière passée en
 * entrée (ou NULL si l'expression passée en entrée n'est pas simple).
 */
nfa *reg_glushkov(regexp * //!< Une expression régulière (simple).
);

#endif // REGAUT_H_
