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
    unsigned int typ;

    /**
     * @brief The argument's data.
     */
    void *data;

};

/**
 * @brief Serialize the data before sending it.
 * WARNING: the memory has to be deallocate after usage.
 * @param arg the argument.
 * @return the serialized message.
 */
char *serialize_data(const struct rpc_arg *arg);

/**
 * @brief Serialize the data before sending it.
 * WARNING: the memory has to be deallocate after usage.
 * @param typ the argument type.
 * @param data the data.
 * @return the serialized message.
 */
char *tserialize_data(unsigned int typ, void *data);

#endif /* RPC_DATA_H */

