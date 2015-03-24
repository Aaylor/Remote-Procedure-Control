#ifndef SERVER_H
#define SERVER_H


/**
 * @file s_server.h
 * @brief Header for server
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */

#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <sys/time.h>

#include "libnet.h"

#include "u_log.h"
#include "u_rpc_data.h"
#include "s_mapper.h"
#include "s_function_handler.h"

#define BUFF 256

/**
 * @brief Create the loop waiting for request of the client. Fork before
 * executing request.
 */
void loop_server(void);

/**
 * @brief execute a double fork to verify if after 5 seconds the process is
 * still running. If yes, kill it and return the appropriate error to the
 * client.
 * @param client The socket client on which we are connected.
 */
void gestion_client(int client);

/**
 * @brief Use to execute the main code of a client.
 * @param The client socket which is connected.
 */
void execute_client(int client);

/**
 * @brief verify if the function called is the same as the function that we
 * want.
 * @param client The socket client on which we are connected.
 * @param f The function take on the mapper.
 * @param msg The message send by the client.
 */
void verification_function(int client, struct function_mapper *f, struct message *msg);

/**
 * @brief Search a function on the mapper considering a message given by the
 * client.
 * @param client The socket client on which we are connected.
 * @param msg The message send by the client.
 */
struct function_mapper *search_function(int client, struct message *msg);

/**
 * @brief Function taking a client and send him an error message.
 * @param client The socket client on which we are connected.
 * @param i The integer which represents the type error expected.
 */
void send_error(int client, char c);

/**
 * @brief Function taking a client and send him the return message.
 * @param client The socket client on which we are connected.
 * @param ret The message representing the return value.
 * @param size The size of the message.
 */
void send_answer(int client, char *ret, int size);

/**
 * @brief read a message in the client socket and save that in the msg param.
 * @param client The socket client on which we are connected.
 * @param msg The message on which the data read is saved.
 */
void read_msg(int client, struct message *msg);

#endif /* SERVER_H */

