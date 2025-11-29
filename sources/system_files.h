/***********************/
/* Gestion de fichiers */
/***********************/

#ifndef FILES_H
#define FILES_H

#include "nfa.h"
#include "regexp.h"
#include "system_main.h"
#include "system_parser.h"
#include "system_shell_languages.h"
#include "system_type_basic.h"
#include <json-c/json.h>

#define EXPSIZE    2048

#define OUTPUT_DIR "./outputs"

void reg_to_string(regexp *r, char *buffer);

/***********************************************/
/* Manipulation de structures JSON - Automates */
/***********************************************/

json_object *files_auto_to_json(nfa *);

json_object *files_lang_to_json(regexp *);

nfa *files_json_to_nfa(json_object *);

/************************************/
/* Lecture/écriture dans un fichier */
/************************************/

void files_save_object(object *, char *);

int files_read_object(char *filename, char *varname);

void files_save_session(char *filename);

void files_load_session(char *filename);

#endif
