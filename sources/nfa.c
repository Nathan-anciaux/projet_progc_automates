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
    B->nb_letters = A->nb_letters;
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

    B->state_names = A->state_names;

    return B;
}

nfa *nfa_union(nfa *, nfa *)
{
    return NULL;
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
