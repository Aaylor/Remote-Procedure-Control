
#include <stdlib.h>

#include "s_server.h"

/**
 * @brief Create the loop waiting for request of the client. Fork before
 * executing request.
 */
void loop_server(){
    struct sockaddr addr;
    socklen_t len;
    int serv, client;

    if ((serv = serv_tcpsock("localhost", AF_INET)) < 0){
        /*FIXME*/
        err(EXIT_FAILURE, "Error server\n");
    }

    while(1){
        if ((client=accept(s, &addr, &len)) < 0)
            /*FIXME*/
            perror("Error accept client\n");
        if(fork()==0){
            execute_client(client, serv);
        }
    }

}

void execute_client(int client){
    struct message *msg;
    struct function_mapper *function;

    read_msg(client, msg);
    function = search_function(client, msg);

}

struct function_mapper *search_function(int client, struct message *msg){
    int ret;
    ret = exist_function(function_memory, msg->command);
    if(ret == 0)
        send_error(client, "The given function does not exist")
    else if(ret == -1)
        send_error(client, "The mapper is null");
    return get_function(function_memory, msg->command);
}

void send_error(client, to){
    if(send(client, to, strlen(to), 0) < 0)
        err(EXIT_FAILURE, "error fail to send");
    exit(EXIT_SUCCESS);
}

void read_msg(int client, struct message *msg){
    int size;
    char *from;

    if(recv(client, &size, sizeof(int), 0) < 0)
        err(EXIT_FAILURE, "Error recv\n");

    from = malloc((size+1)*sizeof(char));

    if(recv(client, &from, size*sizeof(char), 0) < 0){
        free(from);
        err(EXIT_FAILURE, "Error recv2\n");
    }

    if(deserialize_message(msg, size, from) == -1)
        err(EXIT_FAILURE, "Err deserialize_message");

    free(from);
}

/**
 * @file s_server.c
 * @brief Main file to handle server
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */


#ifndef UNIT_TEST

/**
 * @brief Entry point.
 * @return .
 */
int main(void)
{
    return EXIT_SUCCESS;
}

#endif

