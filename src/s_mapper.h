#ifndef S_MAPPER_H
#define S_MAPPER_H

/**
 * @file s_mapper.h
 * @brief function mappers
 * @author Tortrat-gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-17
 */



#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "u_rpc_data.h"



#define FUNCTION_NAME_LENGTH 255

#define MAX_FUNCTIONS        256

#define MAX_PARAMETERS       127



/**
 * @brief Pointer to function that returns void.
 */
typedef void  (*void_ptr)();

/**
 * @brief Pointer to function that returns an integer.
 */
typedef int   (*int_ptr)();

/**
 * @brief Pointer to function that returns a char *.
 */
typedef char *(*str_ptr)();

/**
 * @brief Union that contains different pointer to function, according to
 * their return type.
 */
union fun_ptr_u {

    /**
     * @brief The `void` return function.
     */
    void_ptr void_fun;

    /**
     * @brief The `int` return function.
     */
    int_ptr  int_fun;

    /**
     * @brief The `str` return function.
     */
    str_ptr  str_fun;
};


/**
 * @brief Represents a function type containings every information to
 * typechecking, and his pointer.
 */
struct function_t {

    /**
     * @brief The return type.
     */
    char return_type;

    /**
     * @brief Number of arguments.
     */
    int  argc;

    /**
     * @brief Type of each parameter (max 127 parameters).
     */
    char argv[MAX_PARAMETERS];

    /**
     * @brief The pointer to function.
     */
    union fun_ptr_u fun_ptr;

};

/**
 * @brief Map a function type to its name.
 */
struct function_mapper {

    /**
     * @brief The length of name.
     */
    size_t     name_length;

    /**
     * @brief The function's name.
     */
    char name[FUNCTION_NAME_LENGTH + 1];

    /**
     * @brief The function itself, containing types and its pointer.
     */
    struct function_t fun;

};


/**
 * @brief The memory. Containing all saved function.
 */
struct memory {

    /**
     * @brief The number of function saved.
     */
    size_t size;

    /**
     * @brief Array containing every functions.
     */
    struct function_mapper fmap[MAX_FUNCTIONS];

} function_memory;


/**
 * @brief Fill the mapper with every given informations.
 * @param mapper The mapper to fill.
 * @param name The function name.
 * @param return_type The returned type of the function.
 * @param funptr The pointer to the function
 * @param argc The number of parameters.
 * @param ... All the parameters.
 * @return 
 */
int create_function(struct function_mapper *mapper, const char *name,
        char return_type, union fun_ptr_u funptr, int argc, ...);

/**
 * @brief Add the function to the given memory.
 * @param memory The memory.
 * @param mapper The function mapper.
 */
void add_function(struct memory *memory, struct function_mapper mapper);


#endif /* S_MAPPER_H */

