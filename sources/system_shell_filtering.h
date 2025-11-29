/**
 * @file shell_filtering.h
 * @brief Header file for objects filtering in the shell.
 */

#ifndef SHELL_FILTERING_H
#define SHELL_FILTERING_H

#include "system_shell_commands.h"

/**
 * @brief
 *  Displays of all objects of a given type in memory.
 *
 * @return
 * -1 in case of success, -2 in case of error.
 */
int shell_filter_objects(
    com_parameters *pars, //!< The parameters of the command.
    ob_type type,         //!< The type of the objects to display.
    char *str             //!< The string used to call the command.
);

/***********/
/* Sorting */
/***********/

/**
 * @brief
 * Default comparison function for objects. Sorts according to the
 * variable name.
 *
 * @return
 * A negative value if the first object should be placed before the second one,
 * a positive value if the second object should be placed before the first one,
 * and 0 if the two objects are equal.
 */
int object_compare(int, //!< The index of the first object.
                   int  //!< The index of the second object.
);

/**
 * @brief
 * Sorts the array of objects according to the comparison function passed as
 * input.
 *
 * @remark
 * Uses a heap sort algorithm.
 */
void object_sort_array(int (*comp)(int, int) //!< The comparison function.
);

/**
 * @brief
 * Sorts the objects array according to a comparison function given as input.
 */
int shell_sort(com_parameters *pars);

/**************/
/*+ Deletion +*/
/**************/

/**
 * @brief
 * Deletes an object.
 *
 * @remark
 * -1 in case of success, -2 in case of error.
 */
int shell_delete(com_parameters *pars, //!< The parameters of the command.
                 const char *str       //!< The string used to call the command.
);

/**
 * @brief
 * Deletes all objects in memory.
 *
 * @remark
 * -1 in case of success, -2 in case of error.
 */
int shell_delete_all(com_parameters *pars, //!< The parameters of the command.
                     const char *str //!< The string used to call the command.
);

/**
 * @brief
 * Deletes the state names in an NFA.
 *
 * @return
 * -1 in case of success, -2 in case of error.
 */
int shell_reset(com_parameters *pars, //!< The parameters of the command.
                const char *str       //!< string used to call the command.
);

#endif // SHELL_FILTERING_H
