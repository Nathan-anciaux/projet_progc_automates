#include "system_shell_filtering.h"
#include "system_shell_commands.h"
#include "system_shell_errors.h"

int shell_filter_objects(com_parameters *pars, ob_type type, char *str) {
    if (param_retrieve(pars, 0, 0, NULL, NULL, NULL, NULL, NULL, str) == -2) {
        usage_generic(NULL, 0, PAR_NONE, str);
        return -2;
    }
    // int n = com_nbparams(pars);

    // Computes the total number of objects of the given type (skips hidden
    // objects: syntactic monoids, minimal automata, etc.)
    uint tcount = 0;
    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].parent != -1 ||
            (type != DUMMY && objects[i].type != type)) {
            continue;
        }
        tcount++;
    }

    switch (type) {
    case REGEXP:
        printf("#### There are %d regular expressions(s) in memory:\n\n",
               tcount);
        break;
    case NAUTOMATON:
        printf("#### There are %d automata in memory:\n\n", tcount);
        break;
    case DUMMY:
        printf("#### There are %d object(s) in memory:\n\n", tcount);
        break;
    default:
        break;
    }

    // Computes the number of objects satisfying the filters.
    uint count = 1;
    char buffer[256];
    uchar f = 0;

    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].parent != -1 ||
            (type != DUMMY && objects[i].type != type)) {
            continue;
        }

        f++;
        if (type == DUMMY) {
            sprintf(buffer, "#### %d (%s): %s", count,
                    object_types_names[objects[i].type], objects[i].name);
        } else {
            sprintf(buffer, "#### %d: %s", count, objects[i].name);
        }
        fprintf(stdout, "%-45s", buffer);
        count++;
        if (f == 5) {
            fprintf(stdout, "\n");
            f = 0;
            fprintf(stdout, "\n");
        }
    }

    if (f != 0) {
        fprintf(stdout, "\n");
        fprintf(stdout, "\n");
    }

    count--;

    return -1;
}

/***********/
/* Sorting */
/***********/

int object_compare(int i1, int i2) {
    if (i1 == -1 || i2 == -1) {
        return 0;
    }
    if (objects[i1].parent != -1) {
        if (objects[i2].parent != -1) {
            return 0;
        }
        return 1;
    }

    if (objects[i2].parent != -1) {
        return -1;
    }

    return strcmp(objects[i1].name, objects[i2].name);
}

static void max_heapify(int i, int max, int (*comp)(int, int)) {
    if (i < 0 || i > max) {
        return;
    }
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    while ((l <= max && comp(l, i) > 0) || (r <= max && comp(r, i) > 0)) {
        if (r <= max && comp(l, r) < 0) {
            object_swap(i, r);
            i = r;
        } else {
            object_swap(i, l);
            i = l;
        }
        l = 2 * i + 1;
        r = 2 * i + 2;
    }
}

void object_sort_array(int (*comp)(int, int)) {
    int end = nb_objects - 1;
    // Heapify
    for (int i = end; i >= 0; i--) {
        max_heapify(i, end, comp);
    }
    // Sort

    for (int max = end; max > 0; max--) {
        object_swap(0, max);
        max_heapify(0, max - 1, comp);
    }
}

int shell_sort(com_parameters *pars) {
    if (com_nbparams(pars) > 0) {
        shell_error_rangeparams(keywordtostring(KY_SORT), 0, 0);
        return -1;
    }

    object_sort_array(object_compare);
    return -1;
}

/**************/
/*+ Deletion +*/
/**************/

int shell_delete(com_parameters *pars, const char *str) {
    int i;
    bool saved       = false;
    par_type types[] = {PAR_OBJECTV};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    object_free(i);
    return -1;
}

int shell_delete_all(com_parameters *pars, const char *str) {
    if (param_retrieve(pars, 0, 0, NULL, NULL, NULL, NULL, NULL, str) == -2) {
        usage_generic(NULL, 0, PAR_NONE, str);
        return -2;
    }
    object_free_all();
    return -1;
}

int shell_reset(com_parameters *pars, const char *str) {
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATONV};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    nfa_reset_state_names(objects[i].obj_nfa);
    return -1;
}
