#ifndef PARSE_REGEXP_H_
#define PARSE_REGEXP_H_

#include "nfa.h"
#include "regexp.h"
#include "regexp_thompson.h"
#include "shell_test.h"
#include "system_glushkov.h"
#include "system_main.h"
#include "system_printing.h"
#include "system_shell_help.h"
#include "system_shell_languages.h"
#include "system_tools.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int regexp_lex(void);

void regexp_error(regexp **thereg, char *s);

regexp *parse_string_regexp(const char *in);

#endif // PARSE_REGEXP_H_
