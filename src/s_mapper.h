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

#include <stdio.h>

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
    char argv[127];

    /**
     * @brief Union that contains different pointer to function, according to
     * their return type.
     */
    union {

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
    } fun_ptr;

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
    const char name[256];

    /**
     * @brief The function itself, containing types and its pointer.
     */
    const struct function_t fun;

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
    struct function_mapper fmap[256];

};

#endif /* S_MAPPER_H */

