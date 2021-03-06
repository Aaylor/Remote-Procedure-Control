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
#include <sys/un.h>
#include <sys/socket.h>
#include <string.h>
#include <assert.h>

#include "u_log.h"
#include "u_rpc_data.h"

#define HELP                                                            \
    "Available commands :\n\n"                                          \
                                                                        \
    "  ./client --help\n"                                               \
    "  ./client -h\n"                                                   \
    "\tdisplay this help.\n\n"                                          \
                                                                        \
    "  ./client --list\n"                                               \
    "  ./client -l\n"                                                   \
    "\tgives the list of registers functions in the server.\n\n"        \
                                                                        \
    "  ./client --shutdown password\n"                                  \
    "  ./client -s password\n"                                          \
    "\tshutdown the server; by default, password is admin.\n\n"         \
                                                                        \
    "  ./client --command CMD -ret -TYP [-TYP1 ARG1 ... -TYPN ARGN]\n"  \
    "  ./client -c CMD -ret -TYP [-TYP1 ARG1 ... -TYPN ARGN]\n"         \
    "\tCMD is the command name to call.\n"                              \
    "\t-ret TYP is the return rype.\n"                                  \
    "\tTYP has to be int, str or void.\n"                               \
    "\tif TYP is void, ARG must not be set.\n"


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

int transmition(void *ret, struct message *msg);

/**
 * @brief Creates a client unix server and connect it to the server socket
 * binded with the given path.
 * @param sock_path The address of the server socket that the socket will
 * connect.
 * @return The filedescriptor of the resulting client socket if the operation
 * is succesfull, -1 else.
 */
int clt_sock(char *sock_path);

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

