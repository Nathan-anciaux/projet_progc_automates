#ifndef REGEXP_THOMPSON_H_
#define REGEXP_THOMPSON_H_

#include "error.h"
#include "nfa.h"
#include "nfa_intersec.h"
#include "nfa_minimization.h"
#include "regexp.h"
#include "type_boolarray.h"
#include <stdlib.h>

nfa *reg_thompson(regexp *exp);

#endif // REGEXP_THOMPSON_H_
