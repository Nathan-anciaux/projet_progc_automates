#include "system_printing.h"
#include <sys/utsname.h>

#define LINUX_VIEW_COMMAND "| convert - -resize 70% sixel:- | cat"
#define OSX_VIEW_COMMAND   "| ./imgcat -W auto"

static char *view_command(void) {
    struct utsname name;
    uname(&name);
    if (strcmp(name.sysname, "Darwin") == 0) {
        return OSX_VIEW_COMMAND;
    } else {
        return LINUX_VIEW_COMMAND;
    }
}

bool external_viewer = false;

/********************************/
/* Print des arêtes d'un graphe */
/********************************/

void named_lgedges_print_simple(nfa *A, FILE *out) {
    if (!A) {
        return;
    }

    for (int q = 0; q < A->transitions->nb_states; q++) {
        for (int a = 0; a < A->transitions->nb_labels; a++) {
            int start =
                A->transitions->intervals[q * A->transitions->nb_labels + a];
            int end = A->transitions
                          ->intervals[q * A->transitions->nb_labels + a + 1];

            for (int idx = start; idx < end; idx++) {
                int p = A->transitions->to_array[idx];
                if (A->eps_trans &&
                    A->transitions->nb_labels == A->nb_letters + 1 &&
                    a == A->nb_letters) {
                    fprintf(out, "%d -> %d [label = \"%s\"]\n", q, p, "ε");
                } else if (A->letter_names[a] == ' ') {
                    fprintf(out, "%d -> %d [label = \"%s\"]\n", q, p, "␣");
                } else if (A->letter_names[a] == '\"') {
                    fprintf(out, "%d -> %d [label = \"%c\"]\n", q, p, '\"');
                } else {
                    fprintf(out, "%d -> %d [label = \"%c\"]\n", q, p,
                            A->letter_names[a]);
                }
            }
        }
    }
}

void named_lgedges_print(nfa *A, FILE *out) {
    int size               = 0;
    multi_edge *multiedges = ngraph_to_multi_edges(A->transitions, &size);

    for (int i = 0; i < size; i++) { // Boucle sur les états de départ
        fprintf(out, "%d -> %d [label = \"", multiedges[i].in,
                multiedges[i].out);

        bool start = true;
        for (int a = 0; a < A->nb_letters; a++) {
            if (!getval_barray(multiedges[i].lab, a)) {
                continue;
            }

            if (!start) {
                fprintf(out, ",");
            } else {
                start = false;
            }

            if (A->letter_names[a] == ' ') {
                fprintf(out, "%s", "␣");
            } else if (A->letter_names[a] == '\"') {
                fprintf(out, "\\\"");
            } else {
                fprintf(out, "%c", A->letter_names[a]);
            }
        }
        if (A->eps_trans && A->transitions->nb_labels <= A->nb_letters + 1 &&
            getval_barray(multiedges[i].lab, A->nb_letters)) {
            if (!start) {
                fprintf(out, ",");
            } else {
                start = false;
            }
            fprintf(out, "%s", "ε");
        }

        fprintf(out, "\"]\n");
    }
}

/******************/

/* Print d'un NFA */

/******************/
bool nfa_print(nfa *A, FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"\n");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n\n");

    int i = 0;
    int f = 0;

    for (int k = 0; k < A->transitions->nb_states; k++) {
        fprintf(out, "%d [style=solid", k);

        if (A->state_names) {
            fprintf(out, ",label=\"%s\"", A->state_names[k]);
        }
        if (i < A->nb_initials && A->initials[i] == k && f < A->nb_finals &&
            A->finals[f] == k) {
            fprintf(out, ",fillcolor=\"blue:green\",style=filled,shape = "
                         "doublecircle];\n");
            i++;
            f++;
            continue;
        }

        if (i < A->nb_initials && A->initials[i] == k) {
            fprintf(
                out,
                ",fillcolor=\"blue:green\",style=filled,shape = circle];\n");
            i++;
            continue;
        }

        if (f < A->nb_finals && A->finals[f] == k) {
            fprintf(out, ",shape = doublecircle];\n");
            f++;
            continue;
        }
        fprintf(out, ",shape = circle];\n");
    }

#if ADVANCED
    named_lgedges_print(A, out);
#else
    named_lgedges_print_simple(A, out);
#endif

    fprintf(out, "}\n");

    return true;
}

/**************************/
/* Affichage sur le shell */
/**************************/

// Affichage d'un NFA
void nfa_view(nfa *thenfa) {
    char tmp_filename[] = "/tmp/nfa-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "png");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    nfa_print(thenfa, f_tmp);
    // nfa_print(thenfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command =
            multiple_strcat("dot -Tpng ", tmp_filename, view_command(), NULL);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}
