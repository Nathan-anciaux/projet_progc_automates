/*****************************/
/* Gestionnaire de commandes */
/*****************************/
#include "system_shell_commands.h"
#include "system_files.h"
#include "system_shell_errors.h"
#include "system_shell_filtering.h"

int com_get_object(int i, string_chain *thechain) {
    while (thechain != NULL && i != -1) {
        switch (string_to_keyword(thechain->string)) {
        case KY_MINI:
            i = shell_compute_minimal(i);
            break;
        default:
            return -1;
            break;
        }
        thechain = thechain->next;
    }
    return i;
}

int com_apply_command(com_command *thecom, char *varname, com_mode mode,
                      bool *saved) {
    if (thecom == NULL || thecom->main == NULL) {
        shell_error_syntax();
        return -1;
    }
    if (varname && !check_varname(varname)) {
        shell_error_disallowedvar(varname);
        return -1;
    }

    // Initialize the saved variable if it is not NULL (depends on the chosen
    // mode).
    if (saved) {
        switch (mode) {
        case MODE_PRINT:
            *saved = false;
            break;
        case MODE_DEFAULT:
        case MODE_COPY:
            *saved = true;
            break;
        default:
            break;
        }
    }

    // We check if the command starts with a keyword.
    com_keyword key = string_to_keyword(thecom->main->string);

    // If the command does not start with a keyword.
    if (key == KY_NULL) {
        // In that case there should be no parameters.
        if (thecom->params) {
            shell_error_syntax();
            return -1;
        }

        // First case: the command is raw text, we look for a regexp.
        if (thecom->thetype == CMT_RAW) {
            regexp *myexp = parse_string_regexp(thecom->main->string);
            if (myexp == NULL) {
                shell_error_nullpar(thecom);
                return -1;
            }
            switch (mode) {
            case MODE_PRINT:
                reg_print(myexp);
                reg_free(myexp);
                return -1;
                break;
            case MODE_DEFAULT:
            case MODE_COPY:
                return object_add_regexp(varname, myexp);
                break;
            default:
                break;
            }
            return -1;
        }

        // Second case: the command is a variable name.
        if (!check_varname(thecom->main->string)) {
            shell_error_syntax();
            return -1;
        }

        // We try to get the index of the object corresponding to the variable
        // name.
        int i = object_get_from_name(thecom->main->string);
        if (i == -1) {
            shell_error_unknownvar(thecom->main->string);
            return -1;
        }
        int j = com_get_object(i, thecom->main->next);
        if (j < 0) {
            return j;
        }
        switch (mode) {
        case MODE_DEFAULT:
            if (saved) {
                *saved = false;
            }
            return j;
            break;
        case MODE_PRINT:
            shell_view_object(objects + j, true);
            return -1;
            break;
        case MODE_COPY:
            return shell_copy_generic(j, varname);
            break;
        default:
            break;
        }

        shell_error_syntax();
        return -1;
    }

    // If the command starts with a keyword, we check if it is the only one.
    if (thecom->main->next || key == KY_NULL) {
        shell_error_syntax();
        return -1;
    }

    // Commands that do not calculate objects (specific to print mode).
    if (mode == MODE_PRINT) {
        // Other commands that do not calculate objects.
        switch (key) {
        case KY_SAVE:
            return shell_save_to_file(thecom->params, thecom->main->string);
            break;
        case KY_SAVESESSION:
            return shell_save_session(thecom->params, thecom->main->string);
            break;
        case KY_LOADSESSION:
            return shell_load_session(thecom->params, thecom->main->string);
            break;
        case KY_DELETE:
            return shell_delete(thecom->params, thecom->main->string);
            break;
        case KY_RESET:
            return shell_reset(thecom->params, thecom->main->string);
            break;
        case KY_RUN:
            return shell_view_nfa_run(thecom->params, thecom->main->string);
            break;
        case KY_SORT:
            return shell_sort(thecom->params);
            break;
        case KY_LIST:
            return shell_filter_objects(thecom->params, DUMMY,
                                        thecom->main->string);
            break;
        case KY_CLEAR:
            return shell_delete_all(thecom->params, thecom->main->string);
            break;
        case KY_AUTOMATA:
            return shell_filter_objects(thecom->params, NAUTOMATON,
                                        thecom->main->string);
            break;
        case KY_REGEXPS:
            return shell_filter_objects(thecom->params, REGEXP,
                                        thecom->main->string);
            break;
        default:
            break;
        }
    }

    // Commands that calculate objects.
    int k = -1;
    switch (key) {
    case KY_BMCK:
        k = shell_mccluskey_reg(varname, thecom->params, thecom->main->string);
        break;
    case KY_THOMPSON:
        k = shell_thompson_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_GLUSHKOV:
        k = shell_glushkov_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_MIRROR:
        k = shell_mirror_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_TRIMNFA:
        k = shell_trim_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_KLEENE:
        k = shell_kleene_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_ELIMEPS:
        k = shell_elimeps_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_CONCAT:
        k = shell_concat_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_INTERSEC:
        k = shell_intersect_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_UNION:
        k = shell_union_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_DETERMINIZE:
        k = shell_determinize_nfa(varname, thecom->params,
                                  thecom->main->string);
        break;
    case KY_COMPLEMENT:
        k = shell_complement_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_BRZOZO:
        k = shell_brzozowski_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_OPEN:
        k = shell_open_object(varname, thecom->params, thecom->main->string);
        break;

    case KY_SAVE:
    case KY_SAVESESSION:
    case KY_LOADSESSION:
    case KY_DELETE:
    case KY_RESET:
    case KY_SORT:
    case KY_RUN:
        shell_error_noreturn(thecom);
        return -1;
        break;
    default:
        shell_error_syntax();
        return -1;
        break;
    }

    if (k >= 0 && mode == MODE_PRINT) {
        shell_view_object(objects + k, true);
        object_free(k);
        return -1;
    }
    return k;
}

/******************************/
/*+ Parameter interpretation +*/
/******************************/

bool param_getrawtext(com_parameters *pars, int p, const char *str,
                      char **rawtext) {
    com_command *arg = com_getparam(pars, p);
    if (!com_single(arg)) {
        fprintf(stderr,
                "#### Error  : parameter %d in the command \"%s\" is not raw "
                "text.\n",
                p + 1, str);
        return false;
    }
    if (rawtext) {
        *rawtext = arg->main->string;
    }
    return true;
}

bool param_getinteger(com_parameters *pars, int p, const char *str, int *res) {
    com_command *arg = com_getparam(pars, p);
    if (!com_single(arg)) {
        fprintf(stderr,
                "#### Error  : parameter %d in the command \"%s\" is not an "
                "integer.\n",
                p + 1, str);
        return false;
    }
    char *end;
    *res = strtol(arg->main->string, &end, 10);
    if (*end != '\0') {
        fprintf(stderr,
                "#### Error  : parameter %d in the command \"%s\" is not an "
                "integer.\n",
                p + 1, str);
        return false;
    }
    return true;
}

bool param_getlong(com_parameters *pars, int p, const char *str, long *res) {
    com_command *arg = com_getparam(pars, p);
    if (!com_single(arg)) {
        fprintf(stderr,
                "#### Error  : parameter %d in the command \"%s\" is not an "
                "integer.\n",
                p + 1, str);
        return false;
    }
    char *end;
    *res = strtol(arg->main->string, &end, 10);
    if (*end != '\0') {
        fprintf(stderr,
                "#### Error  : parameter %d in the command \"%s\" is not an "
                "integer.\n",
                p + 1, str);
        return false;
    }
    return true;
}

bool param_checkobjtype(int i, par_type type, int p, const char *str) {
    switch (type) {
    case PAR_AUTOMATONV:
        if (i < 0 || (objects[i].type != NAUTOMATON)) {
            fprintf(stderr,
                    "#### Error  : parameter %d in the command \"%s\" is not "
                    "an automaton variable.\n",
                    p + 1, str);
            return false;
        }
        break;
    case PAR_AUTOMATON:
        if (i < 0 || (objects[i].type != NAUTOMATON)) {
            fprintf(stderr,
                    "#### Error  : parameter %d in the command \"%s\" is not "
                    "an automaton.\n",
                    p + 1, str);
            return false;
        }
        break;
    case PAR_REGEXP:
        if (i < 0 || objects[i].type != REGEXP) {
            fprintf(stderr,
                    "#### Error  : parameter %d in the command \"%s\" is not a "
                    "regular expression.\n",
                    p + 1, str);
            return false;
        }
        break;
    case PAR_AUTOREG:
        if (i < 0 ||
            (objects[i].type != NAUTOMATON && objects[i].type != REGEXP)) {
            fprintf(stderr,
                    "#### Error  : parameter %d in the command \"%s\" is not "
                    "an automaton or a regular expression.\n",
                    p + 1, str);
            return false;
        }
        break;
    case PAR_OBJECTV:
        if (i < 0 ||
            (objects[i].type != NAUTOMATON && objects[i].type != REGEXP)) {
            fprintf(stderr,
                    "#### Error  : parameter %d in the command \"%s\" is not "
                    "an object variable.\n",
                    p + 1, str);
            return false;
        }
        break;
    case PAR_OBJECT:
        if (i < 0 ||
            (objects[i].type != NAUTOMATON && objects[i].type != REGEXP)) {
            fprintf(stderr,
                    "#### Error  : parameter %d in the command \"%s\" is not "
                    "an object.\n",
                    p + 1, str);
            return false;
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

int param_retrieve(com_parameters *pars, int n, int f, par_type *types,
                   char **rawtext, int *integers, int *objs, bool *saved,
                   const char *str) {
    int k = com_nbparams(pars);
    if (n == 0 && k > 0) {
        fprintf(stderr,
                "#### Error  : the command \"%s\" takes no parameters.\n", str);
        return -2;
    }
    if (k > n || k < n - f) {
        fprintf(
            stderr,
            "#### Error  : wrong number of parameters in the command \"%s\".\n",
            str);
        return -2;
    }

    int jr = 0, ji = 0, jo = 0;
    bool failed = false;
    for (int i = 0; i < k; i++) {
        com_command *arg = com_getparam(pars, i);
        switch (types[i]) {
        case PAR_RAWTEXT:
            if (!param_getrawtext(pars, i, str, rawtext + jr)) {
                failed = true;
            }
            jr++;
            break;
        case PAR_INTEGER:
            if (!param_getinteger(pars, i, str, integers + ji)) {
                failed = true;
            }
            ji++;
            break;
        case PAR_OBJECTV:
        case PAR_AUTOMATONV:
            if (!com_single(arg) || !check_varname(arg->main->string)) {
                fprintf(stderr,
                        "#### Error  : parameter %d in the command \"%s\" is "
                        "not a variable.\n",
                        i + 1, str);
                failed = true;
            }
            // fall through
        case PAR_AUTOMATON:
        case PAR_REGEXP:
        case PAR_MORPHISM:
        case PAR_RECDEF:
        case PAR_AUTOMOR:
        case PAR_AUTOREG:
        case PAR_REGMOR:
        case PAR_OBJECT:
        case PAR_REGAUTOMOR:
            objs[jo] = com_apply_command(arg, NULL, MODE_DEFAULT, saved + jo);
            if (objs[jo] == -2) {
                failed = true;
            }
            if (!param_checkobjtype(objs[jo], types[i], i, str)) {
                failed = true;
            }
            jo++;
            break;
        default:
            failed = true;
            break;
        }

        if (failed) {
            for (int h = 0; h < jo; h++) {
                if (saved[h] && objs[h] != -1) {
                    object_free(objs[h]);
                }
            }
            break;
        }
    }

    if (failed) {
        return -2;
    }

    return -1;
}

void usage_generic(par_type *types, int n, par_type typeret, const char *str) {
    fprintf(stderr, "#### Usage  : %s", str);
    if (n > 0) {
        if (n == 1) {
            fprintf(stderr, "(<par>");
        } else {
            fprintf(stderr, "(<par1>");
        }
        for (int i = 1; i < n; i++) {
            fprintf(stderr, ", <par%d>", i + 1);
        }
        fprintf(stderr, ")\n");
        for (int i = 0; i < n; i++) {
            if (n == 1) {
                fprintf(stderr, "              <par> is ");
            } else {
                fprintf(stderr, "              <par%d> is ", i + 1);
            }
            switch (types[i]) {
            case PAR_AUTOMATON:
                fprintf(stderr, "an automaton.\n");
                break;
            case PAR_REGEXP:
                fprintf(stderr, "a regular expression.\n");
                break;
            case PAR_MORPHISM:
                fprintf(stderr, "a morphism.\n");
                break;
            case PAR_RECDEF:
                fprintf(stderr, "a recursive definition.\n");
                break;
            case PAR_AUTOMATONV:
                fprintf(stderr, "an automaton variable.\n");
                break;
            case PAR_AUTOMOR:
                fprintf(stderr, "an automaton or a morphism.\n");
                break;
            case PAR_AUTOREG:
                fprintf(stderr, "an automaton or a regular expression.\n");
                break;
            case PAR_REGMOR:
                fprintf(stderr, "a regular expression or a morphism.\n");
                break;
            case PAR_OBJECTV:
                fprintf(stderr, "an object variable.\n");
                break;
            case PAR_OBJECT:
                fprintf(stderr, "an object.\n");
                break;
            case PAR_REGAUTOMOR:
                fprintf(
                    stderr,
                    "a regular expression or an automaton or a morphism.\n");
                break;
            case PAR_CLASS:
                fprintf(stderr, "a class name.\n");
                break;
            case PAR_RAWTEXT:
                fprintf(stderr, "raw text.\n");
                break;
            case PAR_INTEGER:
                fprintf(stderr, "an integer.\n");
                break;
            default:
                fprintf(stderr, "ERROR.\n");
                break;
            }
        }
    } else {
        fprintf(stderr, "\n");
    }

    switch (typeret) {
    case PAR_AUTOMATON:
        fprintf(stderr, "#### Return : an automaton.\n");
        break;
    case PAR_REGEXP:
        fprintf(stderr, "#### Return : a regular expression.\n");
        break;
    case PAR_MORPHISM:
        fprintf(stderr, "#### Return : a morphism.\n");
        break;
    case PAR_RECDEF:
        fprintf(stderr, "#### Return : a recursive definition.\n");
        break;
    case PAR_NONE:
        fprintf(stderr, "#### Return : nothing.\n");
        break;
    default:
        fprintf(stderr, "#### Return : ERROR.\n");
        break;
    }
}

int com_integer_par(com_parameters *pars, int p, const char *str, int *res) {
    com_command *arg = com_getparam(pars, p);
    if (!com_single(arg)) {
        error_parinteger(str, p);
        return -2;
    }
    char *end;
    *res = strtol(arg->main->string, &end, 10);
    if (*end != '\0') {
        error_parinteger(str, p);
        return -2;
    }
    return -1;
}

/********************************/
/*+ Computation of new objects +*/
/********************************/

int shell_mccluskey_reg(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_REGEXP, str);
        return -2;
    }
    regexp *exp = nfa_mccluskey(objects[i].obj_nfa);

    if (saved) {
        object_free(i);
    }
    return object_add_regexp(varname, exp);
}

int shell_thompson_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_REGEXP};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    // Computes the automaton from the regular expression.
    nfa *automaton = reg_thompson(objects[i].exp);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_glushkov_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_REGEXP};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    // Computes the automaton from the regular expression.
    nfa *automaton = reg_glushkov(objects[i].exp);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_mirror_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    nfa *automaton = nfa_mirror(objects[i].obj_nfa);

    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_union_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int inds[2];
    bool saved[]     = {false, false};
    par_type types[] = {PAR_AUTOMATON, PAR_AUTOMATON};
    if (param_retrieve(pars, 2, 0, types, NULL, NULL, inds, saved, str) == -2) {
        usage_generic(types, 2, PAR_AUTOMATON, str);
        return -2;
    }
    nfa *automaton =
        nfa_union(objects[inds[0]].obj_nfa, objects[inds[1]].obj_nfa);
    if (saved[0]) {
        object_free(inds[0]);
    }
    if (saved[1]) {
        object_free(inds[1]);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_intersect_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int inds[2];
    bool saved[]     = {false, false};
    par_type types[] = {PAR_AUTOMATON, PAR_AUTOMATON};
    if (param_retrieve(pars, 2, 0, types, NULL, NULL, inds, saved, str) == -2) {
        usage_generic(types, 2, PAR_AUTOMATON, str);
        return -2;
    }

    nfa *automaton =
        nfa_intersect(objects[inds[0]].obj_nfa, objects[inds[1]].obj_nfa, true);
    if (saved[0]) {
        object_free(inds[0]);
    }
    if (saved[1]) {
        object_free(inds[1]);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_concat_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int inds[2];
    bool saved[]     = {false, false};
    par_type types[] = {PAR_AUTOMATON, PAR_AUTOMATON};
    if (param_retrieve(pars, 2, 0, types, NULL, NULL, inds, saved, str) == -2) {
        usage_generic(types, 2, PAR_AUTOMATON, str);
        return -2;
    }
    nfa *automaton =
        nfa_concat(objects[inds[0]].obj_nfa, objects[inds[1]].obj_nfa);
    if (saved[0]) {
        object_free(inds[0]);
    }
    if (saved[1]) {
        object_free(inds[1]);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_kleene_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    nfa *automaton = nfa_star(objects[i].obj_nfa);

    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_elimeps_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }
    nfa *automaton = nfa_elimeps(objects[i].obj_nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_trim_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }
    nfa *automaton = nfa_trim(objects[i].obj_nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_determinize_nfa(char *varname, com_parameters *pars,
                          const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    nfa *automaton = nfa_determinize(objects[i].obj_nfa, true);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_complement_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    nfa *automaton = nfa_complement(objects[i].obj_nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_brzozowski_nfa(char *varname, com_parameters *pars, const char *str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON};
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }
    nfa *automaton = nfa_brzozowski(objects[i].obj_nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

/*******************/
/*+ File handling +*/
/*******************/

static void usage_open(const char *str) {
    fprintf(stderr, "#### Usage  : %s(\"<path>\")\n", str);
    fprintf(stderr, "####          <path> is the path to a json file that "
                    "stores an object.\n");
    fprintf(stderr, "#### Return : the object read in the file.\n");
}
int shell_open_object(char *varname, com_parameters *pars, const char *str) {
    char *filename;
    par_type types[] = {PAR_RAWTEXT};
    if (param_retrieve(pars, 1, 0, types, &filename, NULL, NULL, NULL, str) ==
        -2) {
        usage_open(str);
        return -2;
    }
    return files_read_object(filename, varname);
}

static void usage_save(const char *str) {
    fprintf(stderr, "#### Usage  : %s(<par>,\"<path>\")\n", str);
    fprintf(stderr,
            "####          <par> is a regular expression or an automaton.\n");
    fprintf(stderr, "####          <path> is the path to the json file were "
                    "the object has to be saved.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_save_to_file(com_parameters *pars, const char *str) {
    char *filename;
    int i;
    bool saved       = false;
    par_type types[] = {PAR_AUTOREG, PAR_RAWTEXT};
    if (param_retrieve(pars, 2, 0, types, &filename, NULL, &i, &saved, str) ==
        -2) {
        usage_save(str);
        return -2;
    }
    fprintf(stdout, "#### saving in the file: \"%s\".\n", filename);
    files_save_object(objects + i, filename);
    if (saved) {
        object_free(i);
    }
    return -1;
}

static void usage_open_session(const char *str) {
    fprintf(stderr, "#### Usage  : %s(\"<path>\")\n", str);
    fprintf(stderr, "####          <path> is the path to a json file that "
                    "stores a session.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_load_session(com_parameters *pars, const char *str) {
    char *filename;
    par_type types[] = {PAR_RAWTEXT};
    if (param_retrieve(pars, 1, 0, types, &filename, NULL, NULL, NULL, str) ==
        -2) {
        usage_open_session(str);
        return -2;
    }
    printf("loading the session saved in the file: \"%s\".\n", filename);
    files_load_session(filename);
    return -1;
}

static void usage_save_session(const char *str) {
    fprintf(stderr, "#### Usage  : %s(\"<path>\")\n", str);
    fprintf(stderr, "####          <path> is the path to the json file were "
                    "the session has to be saved.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_save_session(com_parameters *pars, const char *str) {
    char *filename;
    par_type types[] = {PAR_RAWTEXT};
    if (param_retrieve(pars, 1, 0, types, &filename, NULL, NULL, NULL, str) ==
        -2) {
        usage_save_session(str);
        return -2;
    }
    printf("saving the session in the file: \"%s\".\n", filename);
    files_save_session(filename);
    return -1;
}

/*************/
/*+ Display +*/
/*************/

static void usage_nfa_runs(const char *str) {
    fprintf(stderr, "#### Usage  : %s(<com>, <word>)\n", str);
    fprintf(stderr,
            "####          <com> is a command evaluating to an automaton.\n");
    fprintf(stderr, "####          <word> is a word.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_view_nfa_run(com_parameters *pars, const char *str) {
    char *raw;
    int k;
    bool saved       = false;
    par_type types[] = {PAR_AUTOMATON, PAR_RAWTEXT};
    if (param_retrieve(pars, 2, 0, types, &raw, NULL, &k, &saved, str) == -2) {
        usage_nfa_runs(str);
        return -2;
    }

    shell_view_object(objects + k, true);
    fprintf(stdout, "Running the word %s in this automaton.\n", raw);

    barray *states = nfa_compute_runs(objects[k].obj_nfa, raw);

    if (states == NULL) {
        fprintf(stdout, "Error: This word contains letters outside the "
                        "alphabet of the automaton.\n");
    } else {
        int count = 0;
        for (int i = 0; i < getsize_barray(states); i++) {
            if (getval_barray(states, i)) {
                count++;
            }
        }
        fprintf(stdout, "Set of states reached: ");
        if (count == 0) {
            printf("∅.\n");
        } else {
            printf("{");
            bool first = true;
            for (int i = 0; i < getsize_barray(states); i++) {
                if (getval_barray(states, i)) {
                    if (!first) {
                        printf(",");
                    }
                    if (objects[k].obj_nfa->state_names) {
                        fprintf(stdout, "%s",
                                objects[k].obj_nfa->state_names[i]);
                    } else {
                        fprintf(stdout, "%d", i);
                    }
                    first = false;
                }
            }
            printf("}.\n");
        }
    }

    if (saved) {
        object_free(k);
    }
    return -1;
}
