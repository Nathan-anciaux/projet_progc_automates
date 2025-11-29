#include "graphs.h"

one_edge *ngraph_to_edge_list(ngraph *g, int *size)
{
    one_edge *arrete_list = malloc(g->intervals[g->nb_states * g->nb_labels] * sizeof(*arrete_list));
    int indice_intervals = 0;
    int index_tab_one_edge = 0;
    for (int etat = 0; etat < g->nb_states; etat++)
    {
        for (int ettiquette = 0; ettiquette < g->nb_labels; ettiquette++)
        {
            for (int i = g->intervals[indice_intervals]; i < g->intervals[indice_intervals + 1]; i++)
            {
                arrete_list[index_tab_one_edge].from = etat;
                arrete_list[index_tab_one_edge].label = ettiquette;
                arrete_list[index_tab_one_edge].to = g->to_array[i];
                index_tab_one_edge++;
            }
            indice_intervals++;
        }
    }
    *size = index_tab_one_edge;
    return arrete_list;
}

ngraph *edge_list_to_ngraph(one_edge *, int, int, int)
{

    return NULL;
}

multi_edge *ngraph_to_multi_edges(ngraph *, int *)
{
    return NULL;
}
