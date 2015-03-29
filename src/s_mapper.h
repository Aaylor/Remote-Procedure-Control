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

#include <sys/queue.h>

#include "u_rpc_data.h"


/**
 * @brief The max length for a function name.
 */
#define FUNCTION_NAME_LENGTH    255

/**
 * @brief The max length for a description.
 */
#define MAX_DESCRIPTION_LENGTH  1023

/**
 *  @brief The max parameters allowed for functions.
 */
#define MAX_PARAMETERS          255



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
typedef union __fun_ptr {

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

} fun_ptr_u;


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
    fun_ptr_u fun_ptr;

};

/**
 * @brief Map a function type to its name.
 */
struct function_mapper {

    /**
     * @brief The length of name.
     */
    size_t name_length;

    /**
     * @brief The function's name.
     */
    char name[FUNCTION_NAME_LENGTH + 1];

    /**
     * @brief The length of description.
     */
    size_t description_length;

    /**
     * @brief Describe the function.
     */
    char description[MAX_DESCRIPTION_LENGTH + 1];

    /**
     * @brief The function itself, containing types and its pointer.
     */
    struct function_t fun;

    /**
     * @brief The type of the function_mapper list.
     */
    LIST_ENTRY(function_mapper) fm_list;

};


/**
 * @brief The memory. Containing all saved function.
 */
struct memory {

    /**
     * @brief The max number before new allocation.
     */
    size_t size;

    /**
     * @brief Boolean to check if the list has already been initialized.
     */
    char initialized;

    /**
     * @brief The structure type corresponding to the list of function_mapper.
     */
    LIST_HEAD(memory_list, function_mapper) fmap;

}
    /**
     * @brief The default function_memory.
     */
    function_memory;

/**
 * @brief Fill the mapper with every given informations.
 * @param mapper The mapper to fill.
 * @param name The function name.
 * @param return_type The returned type of the function.
 * @param description The descrition of the function.
 * If set to NULL, the description is considered as not set.
 * @param funptr The pointer to the function
 * @param argc The number of parameters.
 * @param ... All the parameters.
 * @return 0 if everything is ok.
 */
int create_function(struct function_mapper *mapper, const char *name,
        char return_type, const char *description,
        fun_ptr_u funptr, int argc, ...);

/**
 * @brief Add the function to the given memory.
 * @param memory The memory.
 * @param mapper The function mapper.
 * @return -1 if the given memory is set to NULL.
 */
int add_function(struct memory *memory, struct function_mapper mapper);

/**
 * @brief Check if the function exists into the given memory.
 * @param memory The memory.
 * @param fun_name The function name to search.
 * @return 1 if the function exist, 0 if not, -1 if the given memory is NULL.
 */
int exist_function(struct memory *memory, const char *fun_name);

/**
 * @brief Return the function mapper with the given name.
 * @param memory The memory.
 * @param fun_name The function name to find.
 * @return NULL if it's not exists OR if memory is NULL.
 */
struct function_mapper *get_function(struct memory *memory,
        const char *fun_name);

/**
 * @brief Remove the asked function.
 * @param memory The memory.
 * @param fun_name The function to remove.
 * @return 0 if OK, -1 else.
 */
int remove_function(struct memory *memory, const char *fun_name);

/**
 * @brief Remove every function in the given memory.
 * @param memory The memory.
 */
void clear_mapper(struct memory *memory);



#ifdef DEBUGLOG

/**
 * @brief Print the current memory state.
 * @param memory The memory.
 */
void __print_memory_state(struct memory *memory);

/**
 * @brief Print the current mapper state.
 * @param mapper The mapper.
 */
void __print_function_mapper_state(struct function_mapper *mapper);

/**
 * @brief Print the current function.
 * @param ft The function.
 */
void __print_function_type_state(struct function_t *ft);

#endif


#endif /* S_MAPPER_H */

