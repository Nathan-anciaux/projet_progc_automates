#include "system_shell_languages.h"

#include "error.h"
#include "nfa.h"
#include "regexp.h"
#include "system_printing.h"
#include "system_shell_errors.h"

object *objects                 = NULL;
int nb_objects                  = 0;
int nb_objemax                  = 0;

char *object_types_names[DUMMY] = {"empty object", "regexp", "automaton"};

void init_objects_array(void) {
    CREATE_ZERO_MEMORY(objects, 1024);
    nb_objects = 0;
    nb_objemax = 1024;
}

void grow_objects_array(void) {
    if (nb_objects >= nb_objemax) {
        nb_objemax *= 2;
        REDIMENSION_MEMORY(objects, nb_objemax);
    }
}

/************************/
/* Création/Suppression */
/************************/

int object_init(const char *name) {
    if (name && strlen(name) >= NAME_MAXSIZE) {
        fprintf(stderr, "Error: object name too long (max size: %d).\n",
                NAME_MAXSIZE - 1);
        return -1;
    }
    grow_objects_array();
    int i = nb_objects++;

    if (name) {
        sprintf(objects[i].name, "%s", name);
    } else {
        objects[i].name[0] = '\0';
    }
    objects[i].type   = EMPTYOBJ;
    objects[i].parent = -1;
    objects[i].depend = -1;
    return i;
}

void object_swap(int i, int j) {
    if (i < 0 || i >= nb_objects || j < 0 || j >= nb_objects) {
        fprintf(stderr, "Error: tried to swap an invalid object.\n");
        return;
    }

    if (i == j) {
        return;
    }

    // An array to store the objects connected to the two that are going to be
    // swapped.
    int tab[2 + 2];

    // The parents of the two objects.
    tab[0] = objects[i].parent;
    int s  = 1;
    if (tab[0] != objects[j].parent) {
        tab[s++] = objects[j].parent;
    }

    // The dependencies of the two objects.

    tab[s++] = objects[i].depend;
    tab[s++] = objects[j].depend;

    for (uchar g = 0; g < s; g++) {
        int k = tab[g];
        if (k == -1) {
            continue;
        }
        if (objects[k].parent == i) {
            objects[k].parent = j;
        } else if (objects[k].parent == j) {
            objects[k].parent = i;
        }

        if (objects[k].depend == i) {
            objects[k].depend = j;
        } else if (objects[k].depend == j) {
            objects[k].depend = i;
        }
    }
    object temp = objects[i];
    objects[i]  = objects[j];
    objects[j]  = temp;
}

void object_free_aux(object *theob) {
    if (!theob) {
        return;
    }

    // free(theob->name);

    switch (theob->type) {
    case REGEXP:
        reg_free(theob->exp);
        break;
    case NAUTOMATON:
        nfa_delete(theob->obj_nfa);
        break;
    default:
        break;
    }
    theob->type = EMPTYOBJ; // Clear the pointer to the deleted object
}

void object_free(int i) {
    if (i < 0 || i >= nb_objects) {
        // fprintf(stderr, "%d Error: tried to delete an invalid object.\n", i);
        return;
    }

    object_swap(i, nb_objects - 1);
    i           = nb_objects - 1;
    uchar count = 1;
    if (objects[i].depend != -1) {
        object_swap(objects[i].depend, i - count);
        count++;
    }

    for (uint h = 0; h < count; h++) {
        object_free_aux(&objects[i - h]);
        objects[i - h].type = EMPTYOBJ;
        nb_objects--;
    }
}

void object_free_all(void) {
    for (int i = 0; i < nb_objects; i++) {
        object_free_aux(&objects[i]);
        objects[i].type = EMPTYOBJ;
    }
    nb_objects = 0;
}

/************************/
/* Get/insert an object */
/************************/

char full_name_buffer[256];

int object_get_from_name(const char *name) {
    if (!name) {
        return -1;
    }
    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].parent == -1 && strcmp(objects[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int object_delete_from_name(const char *name) {
    int i = object_get_from_name(name);
    if (i == -1 || objects[i].parent != -1) {
        return -1;
    }
    object_free(i);
    return i;
}

int object_add_regexp(const char *name, regexp *theregexp) {
    if (theregexp == NULL) {
        shell_error_null();
        return -1;
    }

    if (name != NULL) {
        int i = object_get_from_name(name);
        if (i != -1) {
            object_free(i);
        }
    }

    grow_objects_array();
    int i           = object_init(name);
    objects[i].type = REGEXP;
    objects[i].exp  = theregexp;
    return i;
}

int object_add_automaton_nfa(const char *name, nfa *A) {
    if (A == NULL) {
        shell_error_null();
        return -1;
    }

    if (name != NULL) {
        int i = object_get_from_name(name);
        if (i != -1) {
            object_free(i);
        }
    }

    grow_objects_array();

    int i              = object_init(name);

    objects[i].type    = NAUTOMATON;
    objects[i].obj_nfa = A;
    return i;
}

int shell_copy_generic(int i, char *newname) {
    if (i == -1) {
        return -1;
    }
    switch (objects[i].type) {
    case NAUTOMATON:
        return object_add_automaton_nfa(newname, nfa_copy(objects[i].obj_nfa));
        break;
    case REGEXP:
        return object_add_regexp(newname, reg_copy(objects[i].exp));
    default:
        return -1;
        break;
    }
}

/***********************************************/
/* Computing information on an existing object */
/***********************************************/

int shell_compute_minimal(int i) {
    if (i < 0 || i > nb_objects - 1 || objects[i].type < REGEXP) {
        fprintf(stderr, "Error: invalid object.\n");
        return -1;
    }

    if (objects[i].parent != -1) {
        return shell_compute_minimal(objects[i].parent);
    }

    if (objects[i].depend != -1) {
        return objects[i].depend;
    }

    nfa *mini = NULL;
    switch (objects[i].type) {
    case REGEXP: {
        nfa *A = reg_thompson(objects[i].exp);
        mini   = nfa_brzozowski(A);
        nfa_delete(A);
    } break;
    case NAUTOMATON:
        mini = nfa_brzozowski(objects[i].obj_nfa);
        break;
    default:
        break;
    }

    int j = object_add_automaton_nfa(NULL, mini);

    if (j == -1) {
        nfa_delete(mini);
        return j;
    }
    objects[i].depend = j;
    objects[j].parent = i;

    return j;
}

/***********/
/* Display */
/***********/

void shell_view_object(object *ob, bool title) {
    if (!ob) {
        return;
    }
    switch (ob->type) {
    case REGEXP:
        if (title) {
            print_title_box(100, true, stdout, 1, "Regular expression");
        }
        reg_print(ob->exp);
        return;
        break;
    case NAUTOMATON:
        if (title) {
            print_title_box(100, true, stdout, 1, "Automaton");
        }
        nfa_view(ob->obj_nfa);
        return;
        break;

    default:
        return;
        break;
    }
}
