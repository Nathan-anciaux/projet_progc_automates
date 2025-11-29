#include "system_shell_help.h"

#define PAD1 30
#define PAD2 33
#define PADC 8

void help(void) {
    // FILE *p = popen("less", "w");
    // if (p == NULL)
    FILE *p = stdout;

    print_dtitle_box(100, true, p, 1,
                     "Bienvenue dans l'aide du programme LEA !");
    print_title_box(100, true, p, 1, "Commandes de base");
    fprintf(p, "%4s%-*sAffiche cette aide.\n", "", PAD1, "help");
    fprintf(p, "%4s%-*sQuitte le programme.\n", "", PAD1,
            "quit ou exit ou Ctrl-D");
    fprintf(p, "%4s%-*sAffiche les expressions définies.\n", "", PAD1,
            "regexps");
    fprintf(p, "%4s%-*sAffiche les automates définis.\n", "", PAD1, "automata");
    fprintf(p, "%4s%-*sTrie tous les objets par leur nom.\n", "", PAD1, "sort");
    fprintf(p, "%4s%-*sSupprime un objet (expression ou automate).\n", "", PAD1,
            "delete(<variable>)");
    fprintf(p, "%4s%-*sSauve tous les objets définis dans un fichier.\n", "",
            PAD1, "savesession(\"<filename>\")");
    fprintf(p, "%4s%-*sCharge tous les objets définis dans un fichier.\n", "",
            PAD1, "loadsession(\"<filename>\")");
    fprintf(p, "%4s%-*sExécute la fonction test() de shell_test.c.\n", "", PAD1,
            "test");
    fprintf(p, "\n");
    print_title_box(100, true, p, 1,
                    "Utilisation de variables d'expressions régulières");
    fprintf(p, "\n#### Définition, mémorisation d'une expression régulière "
               "dans une variable :\n\n");
    fprintf(p, "%8s<variable> = \"<regexp>\"\n\n", "");
    fprintf(p,
            "%4sLes guillemets sont obligatoires.\n"
            "%4sNoms de variables : commencent par une majuscule, "
            "éventuellement suivie par lettres, chiffres et '_'\n\n",
            "", "");
    fprintf(p,
            "%4sSyntaxe des expressions régulières (généralisées avec "
            "complément et intersection) :\n",
            "");
    fprintf(p,
            "%12se := e + e | e e | e* | !e | e & e | (e) | \\1 | \\0 | "
            "{<variable>}\n",
            "");
    fprintf(p, "%8soù,\n", "");
    fprintf(p,
            "%10s- \\0 représente ∅, \\1 représente ε, ! représente le "
            "complément, & représente l'intersection.\n",
            "");
    fprintf(p, "%10s- Les espaces ne sont pas significatifs.\n", "");
    fprintf(p, "%10s- {L} désigne l'expression mémorisée dans la variable L.\n",
            "");
    fprintf(p,
            "%10s- Pour utiliser une lettre parmi ' ', '\"', '+', '{', '}', "
            "'(', ')', '+', '*', '!' ou '&',\n",
            "");
    fprintf(p,
            "%10s  la faire précéder d'un caractère '\\' (exemple : \\+ "
            "représente la lettre '+').\n\n",
            "");

    fprintf(p, "\n#### Manipulation d'expressions régulières :\n\n");
    fprintf(p, "%4s%-*sAffiche l' expression régulière dans la variable L.\n",
            "", PAD1, "L");
    fprintf(p,
            "%4s%-*sL'automate minimal de L : fonctionne comme une variable "
            "d'automate.\n",
            "", PAD1, "L.minimal");
    fprintf(p, "\n");

    print_title_box(100, true, p, 1, "Utilisation de variables d'automates");
    fprintf(p, "\n#### Définition, mémorisation d'un automate dans une "
               "variable A :\n\n");
    fprintf(p, "%4s%-*sFait une copie de l'automate B.\n", "", PAD1, "A = B");
    fprintf(p,
            "%4s%-*sAlgorithme de Brzozowski-McCluskey (A doit être un "
            "automate).\n",
            "", PAD1, "mccluskey(A)");
    fprintf(p,
            "%4s%-*sAlgorithme de Thompson (L doit être spécifié par une "
            "expression régulière).\n",
            "", PAD1, "thompson(L)");
    fprintf(p,
            "%4s%-*sAlgorithme de Glushkov (L doit être spécifié par une "
            "expression régulière simple).\n",
            "", PAD1, "glushkov(L)");
    fprintf(p,
            "%4s%-*sÉlimination de tous les états qui sont non accessibles ou "
            "non co-accessibles de l'automate B.\n",
            "", PAD1, "trim(B)");
    fprintf(p, "%4s%-*sUnion non déterministe des automates B1 et B2.\n", "",
            PAD1, "union(B1, B2)");
    fprintf(p, "%4s%-*sIntersection des automates B1 et B2.\n", "", PAD1,
            "intersection(B1, B2)");
    fprintf(p, "%4s%-*sConcaténation des automates B1 et B2.\n", "", PAD1,
            "concatenation(B1, B2)");
    fprintf(p, "%4s%-*sÉtoile de Kleene de l'automate B.\n", "", PAD1,
            "kleene(B)");
    fprintf(p, "%4s%-*sDéterminisation de l'automate B.\n", "", PAD1,
            "determinization(B)");
    fprintf(
        p,
        "%4s%-*sMinimisation de l'automate par l'algorithme de Brzozowski.\n",
        "", PAD1, "brzozowski(B)");
    fprintf(p,
            "%4s%-*sSuppression des noms des états dans l'automate B (si il y "
            "en a).\n",
            "", PAD1, "resetnames(B)");

    fprintf(p, "\n#### Manipulation :\n\n");
    fprintf(p, "%4s%-*sAffiche l'automate.\n", "", PAD1, "A");
    fprintf(p,
            "%4s%-*sL'automate minimal de A : fonctionne comme une variable "
            "d'automate.\n",
            "", PAD1, "A.minimal");
    fprintf(p,
            "%4s%-*sCalcule l'ensemble des états de A atteints sur le mot "
            "d'entrée word.\n",
            "", PAD1, "run(A, \"word\")");
    fprintf(p, "%4s%-*sSauvegarde l'automate A dans un fichier.\n", "", PAD1,
            "save(A, \"filename\")");
    fprintf(p, "%4s%-*sCharge dans A le NFA mémorisé dans un fichier.\n", "",
            PAD1, "A = open(\"filename\")");
    fprintf(p, "\n");

    // if (p != stdout)
    // fclose(p);
}
