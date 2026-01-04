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

int compare_one_edge(const void *p1, const void *p2)
{ // L'ordre de priorité est : from > label > to
    const one_edge *oe1 = (const one_edge *)p1;
    const one_edge *oe2 = (const one_edge *)p2;

    if (oe1->from > oe2->from)
    {
        return 1;
    }
    if (oe1->from < oe2->from)
    {
        return -1;
    }
    if (oe1->label > oe2->label)
    {
        return 1;
    }
    if (oe1->label < oe2->label)
    {
        return -1;
    }
    if (oe1->to > oe2->to)
    {
        return 1;
    }
    if (oe1->to < oe2->to)
    {
        return -1;
    }
    return 0;
}

ngraph *edge_list_to_ngraph(one_edge *edges, int size, int nb_states, int nb_labels)
{
    qsort(edges, size, sizeof(one_edge), compare_one_edge);
    ngraph *g = malloc(sizeof(ngraph));
    g->nb_states = nb_states;
    g->nb_labels = nb_labels;
    g->to_array = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
    {
        g->to_array[i] = edges[i].to;
    }
    g->intervals = malloc((nb_states * nb_labels + 1) * sizeof(int));
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

multi_edge *ngraph_to_multi_edges(ngraph *g, int *size)
{
    int *edges_nb = malloc(sizeof(int));                                  // On initialise à NULL un pointeur qu'on va mettre en argument de ngraph_to_edge_list pour qu'il point par la suite vers le nombre d'arète du graphe.
    one_edge *one_edge_list = ngraph_to_edge_list(g, edges_nb);           // C'est plus facile de faire une liste de multi_edges à partir d'une liste de one_edge que à partir d'un ngraph directement car la liste de multi_edges peut se voir comme une forme contracté de liste de one_edge.
    multi_edge *multi_edge_list = malloc(sizeof(multi_edge) * *edges_nb); // edge_nb majore toute les size potentielles.
    *size = 0;                                                            // On va utiliser size comme un compteur

    for (int i = 0; i < *edges_nb; i++)
    {
        int couple_in_multi_edge_list_index = -1;
        for (int j = 0; j < *size; j++)
        { // Je vérifie si le couple dans one_edge_list existe déjà multi_edge_list

            if (one_edge_list[i].from == multi_edge_list[j].in && one_edge_list[i].to == multi_edge_list[j].out)
            {
                couple_in_multi_edge_list_index = j;
            }
        }
        if (couple_in_multi_edge_list_index == -1)
        { // Si le couple n'existe pas, je crée une nouvelle "ligne" correspondante dans multi_edge_list
            *size = *size + 1;
            multi_edge_list[*size - 1].in = one_edge_list[i].from;
            multi_edge_list[*size - 1].out = one_edge_list[i].to;
            multi_edge_list[*size - 1].lab = create_barray(g->nb_labels);
            settrue_barray(multi_edge_list[*size - 1].lab, one_edge_list[i].label);
        }

        if (couple_in_multi_edge_list_index != -1)
        { // Si le couple existe, je change juste le barray pour dire qu'une liaison existe aussi avec une autre étiquette
            settrue_barray(multi_edge_list[couple_in_multi_edge_list_index].lab, one_edge_list[i].label);
        }
    }
    free(one_edge_list);
    free(edges_nb);
    return multi_edge_list;
}
// End Arthur's functions
