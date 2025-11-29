#ifndef PARSER_H_
#define PARSER_H_

#include "grep.h"
#include "nfa.h"
#include "regexp.h"
#include "regexp_thompson.h"
#include "shell_test.h"
#include "system_glushkov.h"
#include "system_main.h"
#include "system_printing.h"
#include "system_shell_commands.h"
#include "system_shell_help.h"
#include "system_shell_languages.h"
#include "system_tools.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int yylex(void);

void shell_error(char *s);

void prompt(void);

void parse_string(const char *in);

#endif // PARSER_H_
