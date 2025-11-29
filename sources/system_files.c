/***********************/
/* Gestion de fichiers */
/***********************/

#include "system_files.h"

static void reg_to_string_aux(regexp *r, char *buffer) {
    TRACE("reg_to_string_aux");

    if (r == NULL) {
        return;
    }

    switch (r->op) {
    case CHAR: {
        char mess[2];
        mess[0] = r->letter;
        mess[1] = '\0';
        strcat(buffer, mess);
    } break;
    case EMPTY:
        strcat(buffer, "0");
        break;
    case EPSILON:
        strcat(buffer, "1");
        break;
    case UNION:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, " + ");
        reg_to_string_aux(r->right, buffer);
        strcat(buffer, ")");
        break;
    case INTER:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, " & ");
        reg_to_string_aux(r->right, buffer);
        strcat(buffer, ")");
        break;
    case CONCAT:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, " ");
        reg_to_string_aux(r->right, buffer);
        strcat(buffer, ")");
        break;
    case STAR:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, ")*");
        break;
    case COMPLEMENT:
        strcat(buffer, "!(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, ")");
        break;
    default:
        CRITICAL("Unknown regexp operator (%d)", r->op);
        break;
    }
}

void reg_to_string(regexp *r, char *buffer) {
    TRACE("reg_to_string");

    if (!r) {
        return;
    }
    reg_to_string_aux(r, buffer);
}

unsigned short autocount = 0;

/***********************************************/
/* Manipulation de structures JSON - Automates */
/***********************************************/

json_object *files_auto_to_json(nfa *A) {
    // Création de l'objet json
    json_object *root = json_object_new_object();
    if (!root) {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return NULL;
    }

    json_object_object_add(root, "TYPE", json_object_new_string("AUTO"));

    int nb_trans;
    one_edge *edges = ngraph_to_edge_list(A->transitions, &nb_trans);

    // Taille de l'alphabet
    json_object_object_add(root, "N_ALPH", json_object_new_int(A->nb_letters));

    // Taille de l'automate (nombre d'états)
    json_object_object_add(root, "N_SIZE", json_object_new_int(A->nb_states));

    // Epsilon transitions flag
    json_object_object_add(root, "EPS_TRANS",
                           json_object_new_int(A->eps_trans ? 1 : 0));

    // Alphabet
    char *alphabet;
    CREATE_MEMORY(alphabet, A->nb_letters + 1);
    for (int i = 0; i < A->nb_letters; i++) {
        alphabet[i] = A->letter_names[i];
    }
    alphabet[A->nb_letters] = '\0';

    json_object_object_add(root, "L_ALPH", json_object_new_string(alphabet));
    free(alphabet);

    // Liste des transitions

    json_object *transj = json_object_new_array_ext(nb_trans);
    for (int i = 0; i < nb_trans; i++) {
        json_object *onetrans = json_object_new_array_ext(3);
        json_object_array_add(onetrans, json_object_new_int(edges[i].from));
        json_object_array_add(onetrans, json_object_new_int(edges[i].label));
        json_object_array_add(onetrans, json_object_new_int(edges[i].to));
        json_object_array_add(transj, onetrans);
    }
    free(edges);
    json_object_object_add(root, "L_TRAN", transj);

    // Liste des états initiaux
    json_object *initials = json_object_new_array_ext(A->nb_initials);
    for (int i = 0; i < A->nb_initials; i++) {
        json_object_array_add(initials, json_object_new_int(A->initials[i]));
    }

    json_object_object_add(root, "L_INIT", initials);

    // Liste des états finaux
    json_object *finals = json_object_new_array_ext(A->nb_finals);
    for (int i = 0; i < A->nb_finals; i++) {
        json_object_array_add(finals, json_object_new_int(A->finals[i]));
    }
    json_object_object_add(root, "L_FINA", finals);

    return root;
}

json_object *files_lang_to_json(regexp *exp) {
    json_object *root = json_object_new_object();
    if (!root) {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return NULL;
    }
    json_object_object_add(root, "TYPE", json_object_new_string("LANG"));

    json_object_object_add(root, "SPEC", json_object_new_string("REXP"));
    char *newexp;
    CREATE_MEMORY(newexp, EXPSIZE);
    newexp[0] = 0;
    reg_to_string(exp, newexp);
    json_object_object_add(root, "REXP", json_object_new_string(newexp));
    free(newexp);
    return root;
}

static int edge_compare_sort(const void *a, const void *b) {
    const one_edge *ea = (const one_edge *)a;
    const one_edge *eb = (const one_edge *)b;
    if (ea->from != eb->from) {
        return ea->from - eb->from;
    }
    if (ea->label != eb->label) {
        return ea->label - eb->label;
    }
    return ea->to - eb->to;
}

static int int_compare(const void *a, const void *b) {
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return (*ia) - (*ib);
}

nfa *files_json_to_nfa(json_object *root) {
    // Récupération de la taille de l'automate
    json_object *jsize_graph;
    if (!json_object_object_get_ex(root, "N_SIZE", &jsize_graph) ||
        json_object_get_type(jsize_graph) != json_type_int) {
        fprintf(stderr, "Error: cannot read the size of the automaton.\n");
        return NULL;
    }
    int size_graph = json_object_get_int(jsize_graph);

    // Récupération de la taille de l'alphabet
    json_object *jsize_alpha;
    if (!json_object_object_get_ex(root, "N_ALPH", &jsize_alpha) ||
        json_object_get_type(jsize_alpha) != json_type_int) {
        fprintf(stderr, "Error: cannot read the size of the alphabet.\n");
        return NULL;
    }
    int size_alpha = json_object_get_int(jsize_alpha);

    // Y a t-il des epsilon transitions ?
    json_object *jeps;
    if (!json_object_object_get_ex(root, "EPS_TRANS", &jeps) ||
        json_object_get_type(jeps) != json_type_int) {
        fprintf(stderr,
                "Error: cannot read whether there are epsilon transitions.\n");
        return NULL;
    }
    bool eps_flag = json_object_get_int(jeps) != 0;

    // Création du NFA
    nfa *A;
    CREATE_ZERO_MEMORY(A, 1);
    A->nb_states   = size_graph;
    A->nb_letters  = size_alpha;
    A->eps_trans   = eps_flag;

    A->state_names = NULL;

    // Récupération de l'alphabet
    json_object *jalph;
    if (!json_object_object_get_ex(root, "L_ALPH", &jalph) ||
        json_object_get_type(jalph) != json_type_string) {
        fprintf(stderr, "Error: cannot read the alphabet.\n");
        free(A);
        return NULL;
    }
    CREATE_MEMORY(A->letter_names, size_alpha);
    const char *alphabet = json_object_get_string(jalph);
    for (int i = 0; i < size_alpha; i++) {
        A->letter_names[i] = alphabet[i];
    }

    // Récupération du graphe des transitions
    json_object *jtrans;
    if (!json_object_object_get_ex(root, "L_TRAN", &jtrans) ||
        json_object_get_type(jtrans) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of transitions.\n");
        nfa_delete(A);
        return NULL;
    }
    int len = json_object_array_length(jtrans);

    one_edge *edges;
    CREATE_MEMORY(edges, len);

    for (int i = 0; i < len; i++) {
        json_object *onetrans = json_object_array_get_idx(jtrans, i);
        if (json_object_get_type(onetrans) != json_type_array ||
            json_object_array_length(onetrans) != 3) {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            free(edges);
            free(A->letter_names);
            free(A);
            return NULL;
        }
        json_object *jq = json_object_array_get_idx(onetrans, 0);
        json_object *ja = json_object_array_get_idx(onetrans, 1);
        json_object *jr = json_object_array_get_idx(onetrans, 2);
        if (json_object_get_type(jq) != json_type_int ||
            json_object_get_type(jr) != json_type_int ||
            json_object_get_type(ja) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            free(edges);
            free(A->letter_names);
            free(A);
            return NULL;
        }
        edges[i].from  = json_object_get_int(jq);
        edges[i].to    = json_object_get_int(jr);
        edges[i].label = json_object_get_int(ja);
    }
    qsort(edges, len, sizeof(one_edge), edge_compare_sort);

    A->transitions =
        edge_list_to_ngraph(edges, len, A->nb_states,
                            A->eps_trans ? A->nb_letters + 1 : A->nb_letters);
    free(edges);

    // Récupération de la liste des états initiaux
    json_object *jini;
    if (!json_object_object_get_ex(root, "L_INIT", &jini) ||
        json_object_get_type(jini) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of initial states.\n");
        nfa_delete(A);
        return NULL;
    }
    len = json_object_array_length(jini);
    CREATE_MEMORY(A->initials, len);
    A->nb_initials = len;
    for (int i = 0; i < len; i++) {
        json_object *jq = json_object_array_get_idx(jini, i);
        if (json_object_get_type(jq) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of final states.\n");
            nfa_delete(A);
            return NULL;
        }
        int q = json_object_get_int(jq);
        if (q < size_graph) {
            A->initials[i] = q;
        } else {
            fprintf(stderr,
                    "Error: incorrect state in the list of initial states.\n");
            nfa_delete(A);
            return NULL;
        }
    }
    qsort(A->initials, A->nb_initials, sizeof(int), int_compare);

    // Récupération de la liste des états finaux
    json_object *jfin;
    if (!json_object_object_get_ex(root, "L_FINA", &jfin) ||
        json_object_get_type(jfin) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of final states.\n");
        nfa_delete(A);
        return NULL;
    }
    len = json_object_array_length(jfin);
    CREATE_MEMORY(A->finals, len);
    A->nb_finals = len;
    for (int i = 0; i < len; i++) {
        json_object *jq = json_object_array_get_idx(jfin, i);
        if (json_object_get_type(jq) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of final states.\n");
            nfa_delete(A);
            return NULL;
        }
        int q = json_object_get_int(jq);
        if (q < size_graph) {
            A->finals[i] = q;
        } else {
            fprintf(stderr,
                    "Error: incorrect state in the list of final states.\n");
            nfa_delete(A);
            return NULL;
        }
    }

    return A;
}

/************************************/
/* Lecture/écriture dans un fichier */
/************************************/

void files_save_object(object *theobj, char *filename) {
    json_object *root = NULL;
    switch (theobj->type) {
    case REGEXP:
        root = files_lang_to_json(theobj->exp);
        break;
    case NAUTOMATON:
        root = files_auto_to_json(theobj->obj_nfa);
        break;
    default:
        return;
        break;
    }
    // Sauvegarde
    if (json_object_to_file(filename, root)) {
        fprintf(stderr, "Error: failed to save %s.\n", filename);
    } else {
        printf("%s saved.\n", filename);
    }

    // Libération du JSON
    json_object_put(root);
}

static void files_read_object_aux(json_object *root, char *varname) {
    json_object *obj_type;
    if (!json_object_object_get_ex(root, "TYPE", &obj_type) ||
        json_object_get_type(obj_type) != json_type_string) {
        fprintf(stderr, "Error: cannot read the type of the object.\n");
        return;
    }
    const char *thetype = json_object_get_string(obj_type);
    if (strcmp(thetype, "AUTO") == 0) {
        fprintf(stdout, "Reading an automaton.\n");
        nfa *A = files_json_to_nfa(root);
        object_add_automaton_nfa(varname, A);
        return;
    }

    if (strcmp(thetype, "LANG") == 0) {
        fprintf(stdout, "Reading a language.\n");
        json_object *lang_spe;
        if (!json_object_object_get_ex(root, "SPEC", &lang_spe) ||
            json_object_get_type(lang_spe) != json_type_string) {
            fprintf(
                stderr,
                "Error: cannot read the specification type of the language.\n");
            return;
        }
        const char *thespec = json_object_get_string(lang_spe);
        if (strcmp(thespec, "REXP") == 0) {
            json_object *theregexp;
            if (!json_object_object_get_ex(root, "REXP", &theregexp) ||
                json_object_get_type(theregexp) != json_type_string) {
                fprintf(stderr, "Error: cannot read the regular expression "
                                "specifying the language.\n");
                return;
            }
            const char *exp = json_object_get_string(theregexp);
            regexp *myexp   = parse_string_regexp(exp);
            if (myexp == NULL) {
                return;
            }
            reg_print(myexp);
            object_add_regexp(varname, myexp);
        }
        if (strcmp(thespec, "AUTO") == 0) {
            json_object *theauto;
            if (!json_object_object_get_ex(root, "AUTO", &theauto) ||
                json_object_get_type(theauto) != json_type_object) {
                fprintf(stderr, "Error: cannot read the automaton specifying "
                                "the language.\n");
                return;
            }
            nfa *A = files_json_to_nfa(theauto);
            char *newname;
            CREATE_MEMORY(newname, 20);
            sprintf(newname, "SYSAUTO%04d", autocount++);
            object_add_automaton_nfa(newname, A);
            return;
        }
        return;
    }
}

int files_read_object(char *filename, char *varname) {
    json_object *root = json_object_from_file(filename);
    if (!root) {
        fprintf(stderr, "Failed to read the file.\n");
        return -2;
    }
    files_read_object_aux(root, varname);
    json_object_put(root);

    return -1;
}

void files_save_session(char *filename) {
    // Création de l'objet json
    json_object *root = json_object_new_object();
    if (!root) {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return;
    }
    json_object_object_add(root, "TYPE", json_object_new_string("SESSION"));
    int size = 0;
    for (int i = 0; i < nb_objects; i++) {
        {
            if (objects[i].parent == -1) {
                size++;
            }
        }
    }

    json_object *thetable = json_object_new_array_ext(size);
    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].parent == -1) {
            json_object *oneobj = json_object_new_array_ext(2);
            json_object_array_add(oneobj,
                                  json_object_new_string(objects[i].name));
            if (objects[i].type == REGEXP) {
                json_object_array_add(oneobj,
                                      files_lang_to_json(objects[i].exp));
            }
            if (objects[i].type == NAUTOMATON) {
                json_object_array_add(oneobj,
                                      files_auto_to_json(objects[i].obj_nfa));
            }
            json_object_array_add(thetable, oneobj);
        }
    }
    json_object_object_add(root, "OBJS", thetable);
    // Sauvegarde
    if (json_object_to_file(filename, root)) {
        fprintf(stderr, "Error: failed to save %s.\n", filename);
    } else {
        printf("%s saved.\n", filename);
    }

    // Libération du JSON
    json_object_put(root);
}

void files_load_session(char *filename) {
    json_object *root = json_object_from_file(filename);
    if (!root) {
        fprintf(stderr, "Failed to read the file.\n");
        return;
    }
    // Récupération du type
    json_object *obj_type;
    if (!json_object_object_get_ex(root, "TYPE", &obj_type) ||
        json_object_get_type(obj_type) != json_type_string) {
        fprintf(stderr, "Error: cannot read the type of the object.\n");
        return;
    }

    const char *thetype = json_object_get_string(obj_type);
    if (strcmp(thetype, "SESSION") != 0) {
        fprintf(stderr, "Error: not a session file.\n");
        return;
    }

    // Récupération des objets
    json_object *jobjs;
    if (!json_object_object_get_ex(root, "OBJS", &jobjs) ||
        json_object_get_type(jobjs) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of objects.\n");
        return;
    }
    int len = json_object_array_length(jobjs);

    for (int i = 0; i < len; i++) {
        json_object *oneobj = json_object_array_get_idx(jobjs, i);
        if (json_object_get_type(oneobj) != json_type_array ||
            json_object_array_length(oneobj) != 2) {
            fprintf(stderr, "Error: cannot read the list of objects.\n");
            return;
        }
        json_object *varname = json_object_array_get_idx(oneobj, 0);
        json_object *theobje = json_object_array_get_idx(oneobj, 1);

        const char *thename  = json_object_get_string(varname);
        files_read_object_aux(theobje, strdup(thename));
    }

    json_object_put(root);
}
