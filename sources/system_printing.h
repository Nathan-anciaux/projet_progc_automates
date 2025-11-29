#ifndef PRINTING_H
#define PRINTING_H

#include "alloc.h"
#include "nfa.h"
#include "parameters.h"
#include "system_tools.h"
#include "system_type_basic.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/********************************/
/* Print des arêtes d'un graphe */
/********************************/

void named_lgedges_print_simple(nfa *A, FILE *out);

void named_lgedges_print(nfa *A, FILE *out);

/******************/
/* Print d'un NFA */
/******************/

bool nfa_print(nfa *A, FILE *out);

/**************************/
/* Affichage sur le shell */
/**************************/

// Affichage d'un NFA
void nfa_view(nfa *nfa);

#endif
