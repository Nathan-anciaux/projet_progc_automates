#ifndef MCCLUSKEY_H
#define MCCLUSKEY_H

#include "nfa.h"
#include "regexp.h"

regexp *nfa_mccluskey(nfa *A);

int index_in_matrix_reg(nfa *A, int i, int j);

regexp *concat_without_epsilon(regexp *r1, regexp *r2);

#endif
