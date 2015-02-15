#ifndef RPC_DATA_H
#define RPC_DATA_H


/**
 * @file rpc_data.h
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


/* SENDER PROTOCOL (client → server):
 *  <message_length : int [4 octet]>
 *  <command_length : int [4 octet]>
 *  <command        : char * [1 octet par charactère]>
 *  <typ            : 1 octet>
 *  <typ1           : 1 octet>
 *  <arg1           : {size of arg : dependant of the type}>
 *  <typ2>          ...
 *  <arg2>          ...
 *  ...
 *  <typN>          ...
 *  <argN>          ...
 */

/* ANSWER PROTOCOL (server → client):
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
 * @brief Represent a data argument.
 * Data is the data of the argument.
 */
struct rpc_arg {

    /**
     * @brief The argument type.
     * It could be RPC_TY_VOID, RPC_TY_INT or RPC_TY_STR.
     */
    int typ : 1;

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
    int return_type : 1;

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
 * @brief Fill the message with given arguments.
 * @param msg the message to fill.
 * @param cmd the command to execute.
 * @param return_type the return type.
 * @param ... every arguments. it hshould be write as TYP, arg.
 * @return 0 if it's correct.
 */
int create_message(struct message *msg, const char *cmd,
        int return_type, int argc, ...);

/**
 * @brief Frees memory from the message.
 * @param msg The message to free.
 */
void free_message(struct message *msg);

/**
 * @brief Serialize the message to be sent through sockets.
 * @param msg the message to serialize.
 * @return the serialized data.
 */
char *serialize_message(struct message *msg);

/**
 * @brief Deserialize data and fill message.
 * @param msg the message to fill.
 * @param serialized_msg the serialized data
 * @return 0 if it's correct.
 */
int deserialize_message(struct message *msg, const char *serialized_msg);


#endif /* RPC_DATA_H */

