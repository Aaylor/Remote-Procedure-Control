
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
            gestion_client(client);
        }
    }

}

/**
 * @brief execute a double fork to verify if after 5 seconds the process is
 * still running. If yes, kill it and return the appropriate error to the
 * client.
 * @param client The socket client on which we are connected.
 */
void gestion_client(int client){
    execute_client(client);
}

/**
 * @brief Use to execute the main code of a client.
 * @param The client socket which is connected.
 */
void execute_client(int client){
    struct message *msg;
    struct function_mapper *function;

    read_msg(client, msg);
    function = search_function(client, msg);
    verification_function(client, function, msg);

}

void execute_function(int client, function_mapper *function, struct message *msg){}

/**
 * @brief verify if the function called is the same as the function that we
 * want.
 * @param client The socket client on which we are connected.
 * @param f The function take on the mapper.
 * @param msg The message send by the client.
 */
void verification_function(int client, function_mapper *f, struct message *msg){
    int i;
    if(f->argc != msg->argc)
        send_error(client, RPC_WRONG_NUMBER_ARGS);
    for(i = 0; i < f->argc; i++){
        if(f->argv[i] != msg->argv[i]->typ)
            send_error(client, RPC_RET_WRONG_ARG);
    }
    if(f->return_type != msg->return_type)
        send_error(client, RPC_RET_WRONG_TYP);
}

/**
 * @brief Search a function on the mapper considering a message given by the
 * client.
 * @param client The socket client on which we are connected.
 * @param msg The message send by the client.
 */
struct function_mapper *search_function(int client, struct message *msg){
    int ret;
    ret = exist_function(function_memory, msg->command);
    if(ret == 0)
        send_error(client, RPC_RET_UNKNOWN_FUNC);
    else if(ret == -1)
        send_error(client, RPC_RET_UNKNOWN_FUNC);
    return get_function(function_memory, msg->command);
}

/**
 * @brief Function taking a client and send him an error message.
 * @param client The socket client on which we are connected.
 * @param i The integer which represents the type error expected.
 */
void send_error(int client, int i){
    if(send(client, &i, sizeof(int), 0) < 0)
        err(EXIT_FAILURE, "error fail to send");
    exit(EXIT_SUCCESS);
}

/**
 * @brief read a message in the client socket and save that in the msg param.
 * @param client The socket client on which we are connected.
 * @param msg The message on which the data read is saved.
 */
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

