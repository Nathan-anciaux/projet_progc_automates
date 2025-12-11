#include "shell_test.h"
#include "nfa.h"
#include "nfa_determi.h"
#include "system_files.h"
#include "system_parse_regexp.h"
#include "system_parser.h"
#include "system_shell_languages.h"
#include "system_tools.h"
#include "graphs.h"
#include <stdio.h>

void test(void)
{
    printf("Hello !\n");
    regexp *myexp = parse_string_regexp("(a(a(ab)*b)*b)*");
    reg_print(myexp);

    // nfa *aut = reg_glushkov(myexp);
    // nfa_view(aut);
    INFO("Test done\n");
}
