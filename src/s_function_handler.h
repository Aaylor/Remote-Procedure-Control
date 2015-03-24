#ifndef S_FUNCTION_HANDLER_H
#define S_FUNCTION_HANDLER_H

#include <string.h>

#include "s_server.h"

/**
 * @brief Take the client a message and a function mapper, and execute the
 * function writing the return in the return_t variable.
 * @param ret The structure which will contains the result.
 * @param client The socket client on which we are connected.
 * @param function The function that we want to execute.
 * @param msg The message given by the client including all of the argument.
 * @return 0 if everything happened correctly.
 */
void execute_function(int client, char **return_t, int *size, struct function_t *function, struct message *msg);

#endif /* S_FUNCTION_HANDLER_H */

