#ifndef CLIENT_H
#define CLIENT_H

/**
 * @file c_client.c
 * @brief Header of clients.
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @author **SAFIR**
 * @version 0.1
 * @date 2015-02-09
 */

#include <stdlib.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <string.h>
#include <assert.h>

#include "libnet.h"

#include "u_log.h"
#include "u_rpc_data.h"


/**
 * @brief Call a certain function *cmd* that will be executed on the server
 * process. The returned value of the function (of type *type*) will be
 * written in the *ret* pointer. The argument of *cmd* and their type must be
 * given as the last arguments followed by a NULL pointer.
 *
 * @param cmd The function that must be executed by the server process
 * @param type The type of the returned value
 * @param ret The pointer in which the returned value will be stored
 * @param ... The arguments that must be given to *cmd*. For each argument,
 * the argument itself must given as a void *, and the type must be given as
 * an int. <b>The list of argument must be terminated by a NULL pointer.</b>
 *
 * @return 0 if the operation is succesfull, -1 else
 */
int external_call
    (const char *cmd, int type, void *ret, ...
     /* void *arg_i, int type_i, NULL */);


/* TODO : Comment this function */
int external_callv
    (const char *cmd, unsigned short argc, struct rpc_arg *argv);

/**
 * @brief Send a message to the server.
 * @param msg The message to send
 * @return the socket filedescriptor if the operation is succesfull, -1 else
 */
int sendCmd(struct message *msg);

/**
 * @brief Recieve the answer from the server and store the return
 * value in the *ret* variable.
 * @param clt The file descriptor that will contain the return message
 * from the server
 * @param type The return value's type expected
 * @param ret The pointer in which th returned value will be stored
 * return 0  if the operation is succesfull, -1 else
 */
int getAnswer(int clt, int type, void *ret);

/**
 * @brief Prints the error corresponding to the given status
 * @param status The status of the error
 */
void printErrorStatus(int status);

#endif /* CLIENT_H */

