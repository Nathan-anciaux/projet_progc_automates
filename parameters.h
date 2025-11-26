#ifndef PARAMETERS_H_
#define PARAMETERS_H_

/**
 * @brief
 * Cette macro influe sur l'affichage des automates.
 *
 * - Si elle est définie à 0, les transitions dont les extrémités sont les mêmes
 *   sont affichées comme plusieurs flèches (autant de flèches que de
 *   transitions).
 *
 * - Sinon, les transitions de mêmes extrémités sont regroupées en une seule
 *   flèche.
 *
 * Pour utiliser la deuxième option, il est nécessaire d'avoir écrit la fonction
 * ngraph_to_multi_edges() du fichier graphs.c.
 */
#define ADVANCED 0

#endif // PARAMETERS_H_
