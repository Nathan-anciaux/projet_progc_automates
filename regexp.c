#include "regexp.h"

bool reg_issimple(regexp *)
{
    return false;
}

void reg_free(regexp *)
{
    return;
}

regexp *reg_copy(regexp *)
{
    return NULL;
}

regexp *reg_empty(void)
{
    return NULL;
}

regexp *reg_epsilon(void)
{
    return NULL;
}

regexp *reg_letter(char)
{
    return NULL;
}

regexp *reg_union(regexp *, regexp *)
{
    return NULL;
}

regexp *reg_inter(regexp *, regexp *)
{
    return NULL;
}

regexp *reg_concat(regexp *, regexp *)
{
    return NULL;
}

regexp *reg_star(regexp *)
{
    return NULL;
}

regexp *reg_complement(regexp *)
{
    return NULL;
}

/* Affichage d'une expression: déjà écrit NE PAS MODIFIER */
static void reg_print_helper(regexp *r, regelem parent)
{
    TRACE("reg_print_helper");

    if (r == NULL)
    {
        return;
    }

    switch (r->op)
    {
    case CHAR:
        if (r->letter == ' ')
        {
            fprintf(stdout, "%s", "␣");
        }
        else
        {
            fprintf(stdout, "%c", r->letter);
        }
        break;

    case EMPTY:
        print_color("∅", BLUE, stdout);
        break;

    case EPSILON:
        print_color("ε", YELLOW, stdout);
        break;

    case UNION:
        DEBUG("Union");

        if (parent != UNION && parent != NONE)
        {
            print_color("(", CYAN, stdout);
        }
        reg_print_helper(r->left, UNION);
        print_color(" + ", RED, stdout);
        reg_print_helper(r->right, UNION);
        if (parent != UNION && parent != NONE)
        {
            print_color(")", CYAN, stdout);
        }
        break;

    case INTER:
        if (parent != INTER && parent != NONE)
        {
            print_color("(", CYAN, stdout);
        }
        reg_print_helper(r->left, INTER);
        printf(" ∩ ");
        reg_print_helper(r->right, INTER);
        if (parent != INTER && parent != NONE)
        {
            print_color(")", CYAN, stdout);
        }
        break;

    case CONCAT:
        DEBUG("Concat");

        if (parent != CONCAT && parent != UNION && parent != NONE)
        {
            print_color("(", CYAN, stdout);
        }
        reg_print_helper(r->left, CONCAT);
        reg_print_helper(r->right, CONCAT);
        if (parent != CONCAT && parent != UNION && parent != NONE)
        {
            print_color(")", CYAN, stdout);
        }
        break;

    case STAR:
        DEBUG("Star");

        if (r->left->op == CHAR || r->left->op == EPSILON || r->left->op == EMPTY)
        {
            reg_print_helper(r->left, STAR);
            print_color("*", CYAN, stdout);
        }
        else
        {
            reg_print_helper(r->left, STAR);
            print_color("*", CYAN, stdout);
        }
        break;

    case COMPLEMENT:
        if (parent != NONE)
        {
            print_color("(", CYAN, stdout);
        }
        print_color("¬", RED, stdout);
        reg_print_helper(r->left, COMPLEMENT);
        if (parent != NONE)
        {
            print_color(")", CYAN, stdout);
        }
        break;

    default:
        CRITICAL("Unknown regexp operator (%d)", r->op);
        break;
    }
}

/* Affichage d'une expression: déjà écrit NE PAS MODIFIER */
void reg_print(regexp *r)
{
    TRACE("reg_print");

    reg_print_helper(r, NONE);
    printf("\n");
}
