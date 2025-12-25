#ifndef NFA_H
#define NFA_H

#include "graphs.h"
#include "system_type_basic.h"
#include "type_boolarray.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int nb_states;
    int nb_letters;
    bool eps_trans;
    ngraph *transitions;
    int *initials;
    int nb_initials;
    int *finals;
    int nb_finals;
    char *letter_names;

    /* Optionnel*/
    char **state_names;
} nfa;

/********************/
/* Gestion des noms */
/********************/

void nfa_reset_state_names(nfa *A);

/*********************************/
/* Initialisation et suppression */
/*********************************/

nfa *create_emptylang(void);

nfa *create_sing_epsilon(void);

nfa *create_sing_letter(char letter);

void nfa_delete(nfa *A);

/***********************************/
/* Opérations simples sur les NFAs */
/***********************************/

nfa *nfa_copy(nfa *A);

nfa *nfa_union(nfa *A1, nfa *A2);

bool is_state_final(nfa *A, int state);

nfa *nfa_concat(nfa *A1, nfa *A2);

nfa *nfa_star(nfa *A);

nfa *nfa_trim(nfa *A);

nfa *nfa_mirror(nfa *A);

nfa *nfa_elimeps(nfa *A);

/***********************/
/* Information sur nfa */
/***********************/

barray *nfa_compute_runs(nfa *A, const char *w);

#endif
