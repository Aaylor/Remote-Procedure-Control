
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
            search_function(client, serv);
        }
    }

}

void execute_client(int client, int serv){
    struct message *msg;

    read_msg(client, msg);

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

