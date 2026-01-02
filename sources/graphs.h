#ifndef GRAPHS_H
#define GRAPHS_H

#include "system_type_basic.h"
#include "type_boolarray.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************/
/*+ Représentations par des tableaux (type utilisé dans les NFAs) +*/
/*******************************************************************/

typedef struct ngraph
{
    int nb_states;
    int nb_labels;
    int *to_array;
    int *intervals;
} ngraph;

/********************************************/
/*+ Représentation par une liste d'arrêtes +*/
/********************************************/

typedef struct
{
    int from;
    int to;
    int label;
} one_edge;

one_edge *ngraph_to_edge_list(ngraph *G, int *size);

int compare_one_edge(const void* p1, const void* p2);

ngraph *edge_list_to_ngraph(one_edge *edges, int size, int nb_states,
                            int nb_labels);

typedef struct
{
    int in;
    barray *lab;
    int out;
} multi_edge;

multi_edge *ngraph_to_multi_edges(ngraph *G, int *size);

bool one_edge_couple_exist_in_multi_edge_list(one_edge *el, multi_edge *me, int *index);

#endif
