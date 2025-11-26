%{
#include <stdio.h>
#include <stdlib.h>

#include "system_main.h"
#include "error.h"
#include "alloc.h"
#include "regexp.h"
#include "system_shell_commands.h"

#include <readline/readline.h>
#include <readline/history.h>

#include "system_parse_regexp.tab.h"

void lex_error(char *s);

void lex_error(char *s){
  fprintf(stderr,"Lexical error in restricted regexp, token %s ignored\n", s);
}

%}

%option noinput nounput

%option noyywrap

LANGNAME \{[[:upper:]][_[:alnum:]]*\}

%%


\n return ';';

[[:space:]] ;

\\["] {
  regexp_lval.carac = '\"';
  return LETTER;
}

"\\ " {
  regexp_lval.carac = ' ';
  return LETTER;
}

"+" {
  return PLUS_TOK;
}

"\\+" {
  regexp_lval.carac = '+';
  return LETTER;
}

"!"  {
  return COMPL_TOK;
}

"\\!" {
  regexp_lval.carac = '!';
  return LETTER;
}

"&"  {
  return INTER_TOK;
}

"\\&" {
  regexp_lval.carac = '&';
  return LETTER;
}


"*" {
  return STAR_TOK;
}

"\\*" {
  regexp_lval.carac = '*';
  return LETTER;
}

"(" {
  return OPEN_TOK;
}

"\\(" {
  regexp_lval.carac = '(';
  return LETTER;
}

")" {
  return CLOSE_TOK;
}

"\\)" {
  regexp_lval.carac = ')';
  return LETTER;
}

"\\1" {
  return EPSILON_TOK;
}

"\\0" {
  return EMPTY_TOK;
}

{LANGNAME} {
  regexp_text[regexp_leng-1] = 0;
  regexp_lval.name = strdup(regexp_text + 1);
  return VARIABLE;
}

"\\{" {
  regexp_lval.carac = '{';
  return LETTER;
}

"\\}" {
  regexp_lval.carac = '}';
  return LETTER;
}


. {
  regexp_lval.carac = regexp_text[0];
  return LETTER;
}

%%

void set_input_string(const char* in);

void end_lexical_scan(void);

void set_input_string(const char * in){
  regexp__scan_string(in);
}

void end_lexical_scan(void){
  regexp_lex_destroy();
}
