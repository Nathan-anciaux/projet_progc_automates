#ifndef NFA_DETERMI_H
#define NFA_DETERMI_H

#include "nfa.h"
#include "system_printing.h"

nfa *nfa_determinize(nfa *A, bool names);

nfa *nfa_complement(nfa *A);

#endif
