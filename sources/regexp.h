#ifndef REGEXP_H_
#define REGEXP_H_

#include "alloc.h"
#include "error.h"
#include "system_tools.h"
#include "system_type_basic.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
    EMPTY,
    EPSILON,
    CHAR,
    UNION,
    INTER,
    COMPLEMENT,
    CONCAT,
    STAR,
    NONE
} regelem;

typedef struct regexp
{
    regelem op;
    struct regexp *left;
    struct regexp *right;
    char letter;
} regexp;

bool reg_issimple(regexp *exp);

void reg_free(regexp *exp);

regexp *reg_copy(regexp *exp);

regexp *reg_empty(void);

regexp *reg_epsilon(void);

regexp *reg_letter(char letter);

regexp *reg_union(regexp *exp1, regexp *exp2);

regexp *reg_inter(regexp *exp1, regexp *exp2);

regexp *reg_concat(regexp *exp1, regexp *exp2);

regexp *reg_star(regexp *exp);

regexp *reg_complement(regexp *exp);

/* Affichage d'une expression: déjà écrit NE PAS MODIFIER */
void reg_print(regexp *exp);

#endif // REGEXP_H_
