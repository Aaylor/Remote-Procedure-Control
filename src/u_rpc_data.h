#ifndef RPC_DATA_H
#define RPC_DATA_H


/**
 * @file u_rpc_data.h
 * @brief Handle data types.
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */


#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "u_log.h"


/* SENDER PROTOCOL (client → server):
 *  <message_length : int [4 octet]>
 *  <command_length : int [4 octet]>
 *  <command        : char * [1 octet par charactère]>
 *  <typ            : 1 octet>
 *  <argc           : int [4 octet]>
 *  <typ1           : 1 octet>
 *  <lg1            : 1 octet>
 *  <arg1           : {size of arg : dependant of the type}>
 *  <typ2>          ...
 *  <lg2>           ...
 *  <arg2>          ...
 *  ...
 *  <typN>          ...
 *  <lgN>           ...
 *  <argN>          ...
 */

/* ANSWER PROTOCOL (server → client):
 *  <message_length : int [4 octet]>
 *  <message_type   : 1 octet>
 * ----- if ret type is OK ----
 *  <arg            : {size of arg : dependant of the type}>
 * ----------------------------
 *  --> after : implements better error msg
 */



/**
 * @brief Represents the type void.
 */
#define RPC_TY_VOID 0x00

/**
 * @brief Represents the type integer.
 */
#define RPC_TY_INT  0x01

/**
 * @brief Representts the type string.
 */
#define RPC_TY_STR  0x02



/**
 * @brief Represents the return code OK.
 */
#define RPC_RET_OK              0x00

/**
 * @brief Return code when the function is unknown.
 */
#define RPC_RET_UNKNOWN_FUNC    0x01

/**
 * @brief Return code when arguments are wrong.
 */
#define RPC_RET_WRONG_ARG       0x02

/**
 * @brief Return code when rpc doesn't answer before 5 seconds.
 */
#define RPC_RET_NO_ANSWER       0x03

/**
 * @brief Return code when type return is wrong.
 */
#define RPC_RET_WRONG_TYP       0x04

/**
 * @brief Return code when the number of argument does not match.
 */
#define RPC_WRONG_NUMBER_ARGS   0x05


/**
 * @brief Represent a data argument.
 * Data is the data of the argument.
 */
struct rpc_arg {

    /**
     * @brief The argument type.
     * It could be RPC_TY_VOID, RPC_TY_INT or RPC_TY_STR.
     */
    char typ;

    /**
     * @brief The argument's data.
     */
    void *data;

};


/**
 * @brief Represent a message.
 */
struct message {

    /**
     * @brief The command length.
     */
    int command_length;

    /**
     * @brief The command to execute.
     */
    char *command;

    /**
     * @brief The return type.
     */
    char return_type;

    /**
     * @brief Number of arguments.
     */
    int argc;

    /**
     * @brief Arguments to send.
     */
    struct rpc_arg *argv;
};


/**
 * @brief Check if the given type exists into the protocol.
 * @param type the type to check
 * @return != 0 if it's exists
 */
int type_exists(char type);

/**
 * @brief Create the message with given arguments.
 * Every array/string will be deep copy.
 * @param msg the message to fill.
 * @param command the command to send.
 * @param return_type the return type.
 * @param argc the number of argument.
 * @param argv arguments. this function make a deep copy of arguments.
 * @return 0 if everything works fine
 */
int create_message(struct message *msg, const char *command,
        char return_type, int argc, struct rpc_arg *argv);


/**
 * @brief Free the memory used by the given message. (It must be created by the
 * function `create_message`)
 * @param msg the message to free.
 */
void free_message(struct message *msg);

/**
 * @brief Serialize an integer into the given buffer.
 * The serialisation write the size of the serialisezd interger
 * and this integer in the giver buffer.
 * The buffer has to be size as the number of digits of i + 1.
 * @param i the number to serialize
 * @param buf the buffer where to serialize
 * @return the number of characters written
 */
int serialize_integer(int i, char *buf);

/**
 * @brief Deserialize the given arugment to get back the integer store in it.
 * @param result pointer where to store the result.
 * @param msg the argument
 * @return the number of character read. -1 if error.
 */
int deserialize_integer(int *result, const char *msg);


/*
 * SENDER PROTOCOL
 */

/**
 * @brief Serialize the message to be sent through sockets, and store its size
 * into the msg_size variable.
 * This function allocates memory which has to be freed.
 * @param msg_size the stored size of the message
 * @param msg the message to serialize.
 * @return the serialized data.
 */
char *serialize_message(int *msg_size, struct message *msg);

/**
 * @brief Deserialize data and fill message.
 * @param msg the message to fill.
 * @param size the message size, use to check if every data has been read.
 * @param serialized_msg the serialized data
 * @return the 
 */
int deserialize_message(struct message *msg, int size,
        const char *serialized_msg);


/*
 * ANSWER PROTOCOL
 */

/**
 * @brief Serialize the answer to be sent through sockets.
 * This function allocates memory. The returning value has to be free.
 * @param msg_size the stored size of the message
 * @param status The return status. Can be RPC_RET_OK, RPC_RET_UNKNOWN_FUNC,
 * RPC_RET_WRONG_ARG, RPC_RET_NO_ANSWER.
 * @param ret the return value to serialize. Can be NULL and will not be used
 * when the return status is RPC_RET_UNKNOWN_FUNC, RPC_RET_WRONG_ARG,
 * RPC_RET_NO_ANSWER.
 * @return the serialized data.
 */
char *serialize_answer(int *msg_size, char status, struct rpc_arg *ret);

/**
 * @brief Deserialize data and fill the return structure.
 * This function allocates memory to contain ret->data if the return value is
 * a string or an int and it must be deallocated.
 * @param ret the return structure to fill.
 * @param size the message size, use to check if every data has been read.
 * @param serialized_ret the serialized data
 * @return the return status. This value can be RPC_RET_OK if the operation
 * was succesfull on the server side. In this case the value returned by the
 * server is set in *ret*. If the return value is greater than 0,
 * (RPC_RET_UNKOWN_FUNC, RPC_RET_WRONG_ARG, RPC_RET_NO_ANSWER,
 * RPC_RET_WRONG_TYP), the operation was unsucessful on the serverside.
 */
int deserialize_answer(struct rpc_arg *ret, int size,
        const char *serialized_ret);




#ifdef DEBUGLOG

/**
 * @brief Log the message into error output.
 * @param msg the message.
 */
void __debug_display_message(struct message *msg);

/**
 * @brief Log the serialized message into error output.
 * @param serialized_msg the serialized message.
 */
void __debug_display_serialized_message(const char *serialized_msg);

/**
 * @brief Log the serialized answer into error output.
 * @param serialized_answer the serialized message.
 */
void __debug_display_serialized_answer(const char *serialized_answer);

#endif


#endif /* RPC_DATA_H */

