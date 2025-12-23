#include "nfa.h"
#include "system_printing.h"

void nfa_reset_state_names(nfa *A)
{
    if (!A || !A->state_names)
    {
        return;
    }

    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        // printf("%d %s\n", A->transitions->nb_states, A->state_names[i]);
        free(A->state_names[i]);
    }
    free(A->state_names);
    A->state_names = NULL;
}

/*********************************/
/* Initialisation et suppression */
/*********************************/
// Begin Nathan's functions
nfa *create_emptylang(void)
{
    // nfa allocation
    nfa *n = calloc(1, sizeof(*n));

    // graph part
    n->transitions = calloc(1, sizeof(*(n->transitions)));
    n->transitions->intervals = NULL;
    n->transitions->to_array = NULL;

    // nfa link with ngraph
    n->nb_states = n->transitions->nb_states;

    // nfa part
    n->finals = NULL;
    n->initials = NULL;
    n->letter_names = NULL;
    n->state_names = NULL;

    return n;
}

nfa *create_sing_epsilon(void)
{
    // nfa allocation
    nfa *n = malloc(sizeof(*n));

    // graph part
    n->transitions = malloc(sizeof(*(n->transitions)));
    n->transitions->nb_labels = 0;
    n->transitions->nb_states = 1;

    n->transitions->intervals = calloc(2, sizeof(*(n->transitions->intervals)));
    n->transitions->to_array = NULL;

    // mfa link with ngraph
    n->nb_states = n->transitions->nb_states;

    // nfa part
    n->nb_letters = 0;
    n->eps_trans = 0;
    n->nb_initials = 1;
    n->nb_finals = 1;

    n->finals = malloc(n->nb_finals * sizeof(*(n->transitions->intervals)));
    n->finals[0] = 0;
    n->initials = malloc(n->nb_initials * sizeof(*(n->transitions->intervals)));
    n->initials[0] = 0;
    n->letter_names = NULL;
    n->state_names = NULL;

    return n;
}
// End Nathan's functions

// Begin Arthur's functions
nfa *create_sing_letter(char letter)
{
    nfa *A = malloc(sizeof(nfa));
    A->nb_states = 2;
    A->nb_letters = 1;
    A->eps_trans = false;
    A->transitions = malloc(sizeof(ngraph));
    A->transitions->nb_states = 2;
    A->transitions->nb_labels = 1;
    A->transitions->to_array = malloc(sizeof(int));
    A->transitions->to_array[0] = 1;
    A->transitions->intervals = malloc(sizeof(int) * ((A->nb_states * A->nb_letters) + 1));
    A->transitions->intervals[0] = 0;
    A->transitions->intervals[1] = 1;
    A->transitions->intervals[2] = 1;
    A->initials = malloc(sizeof(int));
    A->initials[0] = 0;
    A->nb_initials = 1;
    A->finals = malloc(sizeof(int));
    A->nb_finals = 1;
    A->letter_names = malloc(sizeof(char));
    A->letter_names[0] = letter;
    A->state_names = malloc(sizeof(char *) * 2);
    A->state_names[0] = "Initial state";
    A->state_names[1] = "Recognized letter";
    return A;
}

void nfa_delete(nfa *A)
{
    free(A->transitions->to_array);
    free(A->transitions->intervals);
    free(A->transitions);
    free(A->initials);
    free(A->finals);
    free(A->letter_names);
    if (A->state_names != NULL)
    {
        for (int i = 0; i < A->nb_states; i++)
        {
            free(A->state_names[i]);
        }
        free(A->state_names);
    }
    return;
}
// End Arthur's functions
/***********************************/
/* Opérations simples sur les NFAs */
/***********************************/

// Begin Nathan's function
nfa *nfa_copy(nfa *A)
{
    nfa *B = malloc(sizeof(*B));

    B->nb_states = A->nb_states;
    B->eps_trans = A->eps_trans;

    // graph part
    B->transitions = malloc(sizeof(*(B->transitions)));

    B->transitions->nb_states = A->transitions->nb_states;
    B->transitions->nb_labels = A->transitions->nb_labels;

    B->transitions->to_array = malloc(B->transitions->nb_states * sizeof(*(B->transitions->to_array)));

    B->transitions->intervals = malloc(((B->transitions->nb_states * B->transitions->nb_labels) + 1) * sizeof(*(B->transitions->intervals)));

    for (int i = 0; i < B->transitions->nb_states; i++)
    {
        B->transitions->to_array[i] = A->transitions->to_array[i];
    }
    for (int i = 0; i < (B->transitions->nb_states * B->transitions->nb_labels) + 1; i++)
    {
        B->transitions->intervals[i] = A->transitions->intervals[i];
    }

    // initial tab
    B->nb_initials = A->nb_initials;
    B->initials = malloc(B->nb_initials * sizeof(*(B->initials)));
    for (int i = 0; i < B->nb_initials; i++)
    {
        B->initials[i] = A->initials[i];
    }

    // final tab
    B->nb_finals = A->nb_finals;
    B->finals = malloc(B->nb_finals * sizeof(*(B->finals)));
    for (int i = 0; i < B->nb_finals; i++)
    {
        B->finals[i] = A->finals[i];
    }

    // letter tab
    B->nb_letters = A->nb_letters;
    B->letter_names = malloc(B->nb_letters * sizeof(*(B->letter_names)));
    for (int i = 0; i < B->nb_letters; i++)
    {
        B->letter_names[i] = A->letter_names[i];
    }

    // state name part
    if (A->state_names == NULL)
    {
        B->state_names = NULL;
    }
    else
    {
        B->state_names = malloc(A->nb_states * sizeof(*(B->state_names)));
        for (int i = 0; i < B->nb_states; i++)
        {
            B->state_names[i] = A->state_names[i];
        }
    }

    return B;
}

nfa *nfa_union(nfa *A, nfa *B)
{
    nfa *C = malloc(sizeof(*C));

    // letter tab
    C->letter_names = malloc((A->nb_letters + B->nb_letters) * sizeof(*(C->letter_names)));
    for (int i = 0; i < A->nb_letters; i++)
    {
        C->letter_names[i] = A->letter_names[i];
    }
    int u = 0;
    int v = A->nb_letters;
    while (u < B->nb_letters)
    {
        bool is_letter = false;
        for (int j = 0; j < A->nb_letters; j++)
        {
            if (B->letter_names[u] == A->letter_names[j])
            {
                is_letter = true;
            }
        }
        if (is_letter == false)
        {
            C->letter_names[v] = B->letter_names[u];
            v++;
            u++;
        }
        else
        {
            u++;
        }
    }
    C->nb_letters = v;

    // initial tab
    C->nb_initials = A->nb_initials + B->nb_initials;
    C->initials = malloc(C->nb_initials * sizeof(*(C->initials)));
    for (int i = 0; i < A->nb_initials; i++)
    {
        C->initials[i] = A->initials[i];
    }
    for (int i = A->nb_initials; i < C->nb_initials; i++)
    {
        C->initials[i] = B->initials[i - A->nb_initials] + A->nb_states;
    }

    // final tab
    C->nb_finals = A->nb_finals + B->nb_finals;
    C->finals = malloc(C->nb_finals * sizeof(*(C->finals)));
    for (int i = 0; i < A->nb_finals; i++)
    {
        C->finals[i] = A->finals[i];
    }
    for (int i = A->nb_finals; i < C->nb_finals; i++)
    {
        C->finals[i] = B->finals[i - A->nb_finals] + A->nb_states;
    }

    C->nb_states = A->nb_states + B->nb_states;
    C->eps_trans = A->eps_trans || B->eps_trans;

    // state name part
    C->state_names = malloc(C->nb_states * sizeof(*(C->state_names)));
    for (int j = 0; j < A->nb_states; j++)
    {
        C->state_names[j] = A->state_names[j];
    }
    for (int j = 0; j < B->nb_states; j++)
    {
        C->state_names[j + A->nb_states] = B->state_names[j];
    }

    // graph part
    C->transitions = malloc(sizeof(*(C->transitions)));

    C->transitions->nb_states = A->transitions->nb_states + B->transitions->nb_states;
    C->transitions->nb_labels = C->nb_letters;

    C->transitions->intervals = malloc(((C->transitions->nb_states * C->transitions->nb_labels) + 1) * sizeof(*(C->transitions->intervals)));

    // tab creation to help interval tab creation
    int *ind_A = malloc(C->transitions->nb_labels * sizeof(*ind_A));
    int *ind_B = malloc(C->transitions->nb_labels * sizeof(*ind_B));

    for (int i = 0; i < C->transitions->nb_labels; i++)
    {
        ind_A[i] = -1;
        ind_B[i] = -1;

        for (int k = 0; k < A->transitions->nb_labels; k++)
        {
            if (A->letter_names[k] == C->letter_names[i])
            {
                ind_A[i] = k;
                break;
            }
        }
        for (int k = 0; k < B->transitions->nb_labels; k++)
        {
            if (B->letter_names[k] == C->letter_names[i])
            {
                ind_B[i] = k;
                break;
            }
        }
    }

    int index_in_interval = 0;
    int write_in_interval = 0;
    // fill interval tab
    C->transitions->intervals[0] = 0;
    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            int count = 0;
            if (ind_A[j] != -1)
            {
                int a = ind_A[j];
                index_in_interval++;
                count = A->transitions->intervals[(i * A->nb_letters) + a + 1] - A->transitions->intervals[(i * A->nb_letters) + a];
                write_in_interval = write_in_interval + count;
                C->transitions->intervals[index_in_interval] = write_in_interval;
            }
            else
            {
                index_in_interval++;
                C->transitions->intervals[index_in_interval] = write_in_interval;
            }
        }
    }
    for (int i = A->transitions->nb_states; i < C->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            int count = 0;
            if (ind_B[j] != -1)
            {
                int b = ind_B[j];
                index_in_interval++;
                count = B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + b + 1] - B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + b];
                write_in_interval = write_in_interval + count;
                C->transitions->intervals[index_in_interval] = write_in_interval;
            }
            else
            {
                index_in_interval++;
                C->transitions->intervals[index_in_interval] = write_in_interval;
            }
        }
    }

    // fill to_array
    int size_to_array = C->transitions->intervals[(C->transitions->nb_labels * C->transitions->nb_states)];
    C->transitions->to_array = malloc(size_to_array * sizeof(*(C->transitions->to_array)));

    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            if (ind_A[j] != -1)
            {
                int l_a = ind_A[j];

                int deb = A->transitions->intervals[(i * A->nb_letters) + l_a];
                int fin = A->transitions->intervals[((i * A->nb_letters) + l_a) + 1];

                int write_pos = C->transitions->intervals[(i * C->nb_letters) + j];

                for (int k = deb; k < fin; k++)
                {
                    C->transitions->to_array[write_pos] = A->transitions->to_array[k];
                    write_pos++;
                }
            }
        }
    }
    for (int i = A->transitions->nb_states; i < C->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            if (ind_B[j] != -1)
            {
                int l_b = ind_B[j];

                int deb = B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + l_b];
                int fin = B->transitions->intervals[(((i - A->transitions->nb_states) * B->nb_letters) + l_b) + 1];

                int write_pos = C->transitions->intervals[(i * C->nb_letters) + j];

                for (int k = deb; k < fin; k++)
                {
                    C->transitions->to_array[write_pos] = B->transitions->to_array[k] + A->nb_states;
                    write_pos++;
                }
            }
        }
    }

    free(ind_A);
    free(ind_B);
    return C;
}

nfa *nfa_concat(nfa *, nfa *)
{

    return NULL;
}

nfa *nfa_star(nfa *)
{
    return NULL;
}
// End Nathan's function

// Begin Arthur's function
nfa *nfa_trim(nfa *)
{
    return NULL;
}

nfa *nfa_mirror(nfa *)
{
    return NULL;
}

nfa *nfa_elimeps(nfa *)
{
    return NULL;
}

barray *nfa_compute_runs(nfa *, const char *)
{
    return NULL;
}
// End Arthur's function
