#ifndef LANGUAGES_H_
#define LANGUAGES_H_

#include "nfa.h"
#include "nfa_determi.h"
#include "regexp.h"
#include "regexp_thompson.h"
#include "system_glushkov.h"
#include "system_printing.h"
#include <stdlib.h>

/**
 * @brief
 * Structure used to store a variable name prefix.
 */
typedef struct {
    char *name;   //!< The variable name prefix.
    uint count;   //!< The number of objects with this prefix.
    uchar digits; //!< The number of digits used for the suffix.
} ob_prefixname;

#define RECMAX 8

/**
 * @brief
 * The different types of objects.
 */
typedef enum {
    EMPTYOBJ,   //!< Empty object.
    REGEXP,     //!< Regular expression.
    NAUTOMATON, //!< Nondeterministic finite automaton.
    DUMMY,      //!< Empty object (used to handle errors).
} ob_type;

extern char *object_types_names[DUMMY];

#define NAME_MAXSIZE 128 //!< Maximum size of the variable name.

/**
 * @brief
 * Type used to represent an object.
 */
typedef struct {
    char name[NAME_MAXSIZE]; //!< Variable name (NULL if the object is a
                             //!< dependency).
    ob_type type;            //!< Type of the object.
    union {
        regexp *exp;  //!< Case of a regular expression.
        nfa *obj_nfa; //!< Case of a nondeterministic finite automaton.
    };
    int depend; //!< Dependency (-1 if no dependency).
    int parent; //!< Index of the parent object for dependencies (-1 if no
                //!< parent).
} object;

/**
 * @brief
 * The array of objects.
 */
extern object *objects;

/**
 * @brief
 * The number of objects in the array.
 */
extern int nb_objects;

/**
 * @brief
 * Initilization of the array of objects.
 */
void init_objects_array(void);

/**
 * @brief
 * Double the size of the array of objects.
 */
void grow_objects_array(void);

/************************/
/* Creation/Deletion */
/************************/

/**
 * @brief
 * Initialization of a new object in the table. Does not define its type.
 *
 * @return
 * The index of the created object.
 */
int object_init(const char *name //!< The variable name of the object.
);

/**
 * @brief
 * Swap two objects in the table.
 */
void object_swap(int, //!< The index of the first object.
                 int  //!< The index of the second object.
);

/**
 * @brief
 * Auxiliary function to free an object.
 */
void object_free_aux(object * //!< The object to free.
);

/**
 * @brief
 * Release of an object.
 */
void object_free(int //!< Index of the object.
);

/**
 * @brief
 * Release of all objects.
 */
void object_free_all(void);

/************************/
/* Get/insert an object */
/************************/

/**
 * @brief
 * Gets the index of an object from its variable name.
 *
 * @return
 * The index of the object or -1 if the object was not found.
 */
int object_get_from_name(const char * //!< The variable name.
);

/**
 * @brief
 * Deletes an object from its variable name.
 *
 * @return
 * The index of the object that was deleted or -1 if the object was not found.
 */
int object_delete_from_name(const char * //!< The variable name.
);

/**
 * @brief
 * Adds a new object of type regular expression.
 *
 * @attention
 * The name passed as input is duplicated. The regular expression
 * passed as input is NOT duplicated.
 *
 * @return
 * The index of the created object.
 */
int object_add_regexp(const char *, //!< The desired variable name (NULL if no
                                    //!< name for system objects).
                      regexp *      //!< The regular expression.
);

/**
 * @brief
 * Adds a new object of type automaton (NFA structure).
 *
 * @attention
 * The name passed as input is duplicated. The automaton passed
 * as input is NOT duplicated.
 *
 * @return
 * The index of the created object.
 */
int object_add_automaton_nfa(const char *, //!< The desired variable name (NULL
                                           //!< if no name for system objects).
                             nfa *         //!< The automaton.
);

/**
 * @brief
 * Copy of an object.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the copy.
 */
int shell_copy_generic(int i,        //!< The index of the object to copy.
                       char *newname //!< The variable name for the new object.
);

/***********************************************/
/* Computing information on an existing object */
/***********************************************/

/**
 * @brief
 * Computes the minimal automaton of an object (if it has not already been
 * computed).
 *
 * @return
 * The index of the minimal automaton in the object array.
 */
int shell_compute_minimal(int //!< The index of the object for which the minimal
                              //!< automaton needs to be computed.
);

/***********/
/* Display */
/***********/

/**
 * @brief
 * Displays an object
 */
void shell_view_object(
    object *ob, //!< The object.
    bool title  //!< Boolean indicating if a title should be displayed.
);

#endif // LANGUAGES_H_
