#include "system_glushkov.h"

int reg_countletters(regexp *exp)
{
    if (!exp)
    {
        return 0;
    }
    if (exp->op == CHAR)
    {
        return 1;
    }
    return reg_countletters(exp->left) + reg_countletters(exp->right);
}

glushkov_info *reg_create_gk_emp(int size)
{
    glushkov_info *new;
    CREATE_MEMORY(new, 1);
    new->size = size;
    new->epsilon = false;
    if (size)
    {
        new->first = create_barray(size);
        new->last = create_barray(size);
        new->follow = create_barray(size * size);
    }
    else
    {
        new->first = NULL;
        new->last = NULL;
        new->follow = NULL;
    }
    return new;
}

glushkov_info *reg_create_gk_eps(int size)
{
    glushkov_info *new = reg_create_gk_emp(size);
    new->epsilon = true;
    return new;
}

glushkov_info *reg_create_gk_let(int a, int size)
{
    glushkov_info *info = reg_create_gk_emp(size);
    info->epsilon = false;
    settrue_barray(info->first, a);
    settrue_barray(info->last, a);
    return info;
}

glushkov_info *reg_gk_star(glushkov_info *info)
{
    info->epsilon = true;
    for (int i = 0; i < info->size; i++)
    {
        for (int j = 0; j < info->size; j++)
        {
            if (getval_barray(info->last, i) && getval_barray(info->first, j))
            {
                settrue_barray(info->follow, j * info->size + i);
            }
        }
    }
    return info;
}

glushkov_info *reg_gk_union(glushkov_info *infol, glushkov_info *infor)
{
    glushkov_info *infoc;
    CREATE_MEMORY(infoc, 1);
    infoc->size = infol->size;
    infoc->epsilon = infol->epsilon || infor->epsilon;
    infoc->first = or_barray(infol->first, infor->first);
    infoc->last = or_barray(infol->last, infor->last);
    infoc->follow = or_barray(infol->follow, infor->follow);
    reg_gk_delete(infol);
    reg_gk_delete(infor);
    return infoc;
}

glushkov_info *reg_gk_concat(glushkov_info *infol, glushkov_info *infor)
{
    glushkov_info *infoc;
    CREATE_MEMORY(infoc, 1);
    infoc->size = infol->size;

    infoc->epsilon = infol->epsilon && infor->epsilon;
    if (infol->epsilon)
    {
        infoc->first = or_barray(infol->first, infor->first);
    }
    else
    {
        infoc->first = copy_barray(infol->first);
    }

    if (infor->epsilon)
    {
        infoc->last = or_barray(infol->last, infor->last);
    }
    else
    {
        infoc->last = copy_barray(infor->last);
    }

    infoc->follow = or_barray(infol->follow, infor->follow);
    for (int i = 0; i < infol->size; i++)
    {
        for (int j = 0; j < infor->size; j++)
        {
            if (getval_barray(infol->last, i) &&
                getval_barray(infor->first, j))
            {
                settrue_barray(infoc->follow, j * infol->size + i);
            }
        }
    }

    reg_gk_delete(infol);
    reg_gk_delete(infor);
    return infoc;
}

void reg_gk_delete(glushkov_info *info)
{
    if (info)
    {
        delete_barray(info->first);
        delete_barray(info->last);
        delete_barray(info->follow);
        free(info);
    }
}

glushkov_info *gk_indexleaves(regexp *exp, int size, int *i, char *map)
{
    if (!exp)
    {
        return NULL;
    }
    switch (exp->op)
    {
    case EMPTY:
        return reg_create_gk_emp(size);
        break;
    case EPSILON:
        return reg_create_gk_eps(size);
        break;
    case CHAR:
        map[*i] = exp->letter;
        return reg_create_gk_let((*i)++, size);
        break;
    case STAR:
        return reg_gk_star(gk_indexleaves(exp->left, size, i, map));
        break;
    case UNION:
        return reg_gk_union(gk_indexleaves(exp->left, size, i, map),
                            gk_indexleaves(exp->right, size, i, map));
        break;
    case CONCAT:
        return reg_gk_concat(gk_indexleaves(exp->left, size, i, map),
                             gk_indexleaves(exp->right, size, i, map));
        break;
    default:
        break;
    }
    return NULL;
}

static int edge_compare_sort(const void *a, const void *b)
{
    const one_edge *ea = (const one_edge *)a;
    const one_edge *eb = (const one_edge *)b;
    if (ea->from != eb->from)
    {
        return ea->from - eb->from;
    }
    if (ea->label != eb->label)
    {
        return ea->label - eb->label;
    }
    return ea->to - eb->to;
}

nfa *reg_glushkov(regexp *exp)
{
    if (!reg_issimple(exp))
    {
        fprintf(stderr, "Error: extended regular expressions are not "
                        "compatible with Glushkov's algorithm.\n");
        return NULL;
    }

    int nleaves = reg_countletters(exp);
    uchar *tab;
    CREATE_MEMORY(tab, nleaves);
    int num = 0;
    glushkov_info *infos = gk_indexleaves(exp, nleaves, &num, (char *)tab);

    // Calcul de l'alphabet
    barray *alph_set = create_barray(256);
    int count = 0;
    for (int i = 0; i < nleaves; i++)
    {
        if (!getval_barray(alph_set, tab[i]))
        {
            settrue_barray(alph_set, tab[i]);
            count++;
        }
    }

    nfa *A;
    CREATE_MEMORY(A, 1);
    CREATE_MEMORY(A->letter_names, count);
    A->state_names = NULL;
    A->nb_states = nleaves + 1;
    A->nb_letters = count;
    A->eps_trans = false;
    int alpha_num[256];
    int j = 0;
    for (int a = 0; a < 256; a++)
    {
        if (getval_barray(alph_set, a))
        {
            A->letter_names[j] = a;
            alpha_num[a] = j;
            j++;
        }
    }

    delete_barray(alph_set);

    // Initial state
    A->nb_initials = 1;
    CREATE_MEMORY(A->initials, 1);
    A->initials[0] = 0;

    // Final states
    A->nb_finals = 0;
    if (infos->epsilon)
    {
        A->nb_finals++;
    }
    for (int i = 0; i < nleaves; i++)
    {
        if (getval_barray(infos->last, i))
        {
            A->nb_finals++;
        }
    }
    CREATE_MEMORY(A->finals, A->nb_finals);

    int start = 0;
    if (infos->epsilon)
    {
        A->finals[0] = 0;
        start = 1;
    }

    for (int i = 0; i < nleaves; i++)
    {
        if (getval_barray(infos->last, i))
        {
            A->finals[start++] = i + 1;
        }
    }

    // Transitions

    int nb_transitions = 0;

    for (int i = 0; i < nleaves; i++)
    {
        if (getval_barray(infos->first, i))
        {
            nb_transitions++;
        }
    }

    for (int k = 0; k < nleaves * nleaves; k++)
    {
        if (getval_barray(infos->follow, k))
        {
            nb_transitions++;
        }
    }

    one_edge *edges;
    CREATE_MEMORY(edges, nb_transitions);
    int index = 0;
    for (int i = 0; i < nleaves; i++)
    {
        if (getval_barray(infos->first, i))
        {
            int a = alpha_num[tab[i]];
            edges[index].from = 0;
            edges[index].to = i + 1;
            edges[index].label = a;
            index++;
        }
    }
    for (int k = 0; k < nleaves * nleaves; k++)
    {
        if (getval_barray(infos->follow, k))
        {
            int q = k % nleaves + 1;
            int r = k / nleaves + 1;
            int a = alpha_num[tab[r - 1]];
            edges[index].from = q;
            edges[index].to = r;
            edges[index].label = a;
            index++;
        }
    }
    qsort(edges, nb_transitions, sizeof(one_edge), edge_compare_sort);
    A->transitions =
        edge_list_to_ngraph(edges, nb_transitions, A->nb_states, A->nb_letters);
    free(edges);

    reg_gk_delete(infos);
    free(tab);

    return A;
}
