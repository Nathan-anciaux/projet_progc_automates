#include "nfa.h"
#include "system_printing.h"

void nfa_reset_state_names(nfa *A) {
    if (!A || !A->state_names) {
        return;
    }

    for (int i = 0; i < A->transitions->nb_states; i++) {
        // printf("%d %s\n", A->transitions->nb_states, A->state_names[i]);
        free(A->state_names[i]);
    }
    free(A->state_names);
    A->state_names = NULL;
}

/*********************************/
/* Initialisation et suppression */
/*********************************/

nfa *create_emptylang(void) {
    return NULL;
}

nfa *create_sing_epsilon(void) {
    return NULL;
}

nfa *create_sing_letter(char) {
    return NULL;
}

void nfa_delete(nfa *) {
    return;
}

/***********************************/
/* Opérations simples sur les NFAs */
/***********************************/

nfa *nfa_copy(nfa *) {
    return NULL;
}

nfa *nfa_union(nfa *, nfa *) {
    return NULL;
}

nfa *nfa_concat(nfa *, nfa *) {
    return NULL;
}

nfa *nfa_star(nfa *) {
    return NULL;
}

nfa *nfa_trim(nfa *) {
    return NULL;
}

nfa *nfa_mirror(nfa *) {
    return NULL;
}

nfa *nfa_elimeps(nfa *) {
    return NULL;
}

barray *nfa_compute_runs(nfa *, const char *) {
    return NULL;
}
