%{
#include "system_parse_regexp.h"
%}

%parse-param {regexp** thereg}

%code{
void regexp_error(regexp**, char *s){
  if (s)
    fprintf(stderr, "Syntax error in restricted regexp.\n");
}
}

%union {
  regexp *regexp;
  char *name;
  char carac;
}

%type <regexp> e
%token <name>VARIABLE <carac>LETTER <regexp>EPSILON_TOK <regexp>EMPTY_TOK
%token OPEN_TOK CLOSE_TOK
%left PLUS_TOK
%left INTER_TOK
%left CONCAT_TOK
%right STAR_TOK
%nonassoc COMPL_TOK

%%

s : error { *thereg = NULL;}
  | e[exp] { *thereg = $exp; }
  ;

e[res] :  e[exp1] PLUS_TOK e[exp2] { $res = reg_union($exp1, $exp2); }

| e[exp1] e[exp2] %prec CONCAT_TOK { $res = reg_concat($exp1, $exp2); }

| e[exp] STAR_TOK { $res = reg_star($exp); }

| COMPL_TOK e[exp] { $res = reg_complement($exp); }

| e[exp1] INTER_TOK e[exp2] { $res = reg_inter($exp1, $exp2); }

| OPEN_TOK e[exp] CLOSE_TOK { $res = $exp; }

| LETTER[letter] { $res = reg_letter($letter); }

| EPSILON_TOK { $res = reg_epsilon(); }

| EMPTY_TOK { $res = reg_empty(); }

| VARIABLE[var] {
    DEBUG("RVALUE %s", $var);
    int i = object_get_from_name($var);
    if (i == -1 || objects[i].type != REGEXP) {
      fprintf(stderr,"Error: %s is not a valid language variable.\n",$var);
      $res = NULL;
    }
    else {
    DEBUG("Language found");
    $res = reg_copy(objects[i].exp);
  }
}
;

%%

/* Declarations */

void set_input_string(const char* in);
void end_lexical_scan(void);


/* This function parses a string */
regexp* parse_string_regexp(const char* in){
  set_input_string(in);
  regexp* res;
  regexp_parse(&res);
  end_lexical_scan();
  return res;
}
