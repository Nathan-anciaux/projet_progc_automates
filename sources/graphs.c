#include "graphs.h"
// Begin Nathan's functions
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
// End Nathan's functions

// Begin Arthur's functions
ngraph *edge_list_to_ngraph(one_edge *edges, int size, int nb_states, int nb_labels)
{
    ngraph *g = malloc(sizeof(ngraph));
    g->nb_states = nb_states;
    g->nb_labels = nb_labels;
    g->to_array = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
    {
        g->to_array[i] = edges[i].to;
    }
    g->intervals = malloc(sizeof(nb_states * nb_labels + 1));
    g->intervals[0] = 0;
    for (int q = 0; q < nb_states; q++)
    {
        for (int l = 0; l < nb_labels; l++)
        {
            int count = 0;
            for (int i = 0; i < size; i++)
            { // On pourrait peut-être diminuer la complexité ici
                if (edges[i].from == q && edges[i].label == l)
                {
                    count++;
                }
            }
            g->intervals[q * nb_labels + l + 1] = g->intervals[q * nb_labels + l] + count;
        }
    }
    return g;
}

multi_edge *ngraph_to_multi_edges(ngraph *, int *)
{
    return NULL;
}
// End Arthur's functions
