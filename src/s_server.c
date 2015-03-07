
#include <stdlib.h>

#include "s_server.h"

void loop_server(void){
    struct sockaddr addr;
    socklen_t len;
    int serv, client;

    if ((serv = serv_tcpsock("23456", AF_INET)) < 0){
        /*FIXME*/
        err(EXIT_FAILURE, "Error server\n");
    }

    while(1){
        fwrite_log(stderr, "Waiting client.");
        if ((client=accept(serv, &addr, &len)) < 0)
            /*FIXME*/
            perror("Error accept client\n");
        fwrite_log(stderr, "Client accepted.");
        if(fork()==0){
            fwrite_log(stderr, "New process: handling client.");
            gestion_client(client);
        }
    }

}

void gestion_client(int client){
    execute_client(client);
}

void execute_client(int client){
    struct message msg;
    struct function_mapper *function;

    fwrite_log(stderr, "Reading client message.");
    read_msg(client, &msg);

    fwrite_log(stderr, "Search the function queried.");
    function = search_function(client, &msg);

    fwrite_log(stderr, "Function verification.");
    verification_function(client, function, &msg);
}

void execute_function(int client, void *return_t, struct function_t *function, struct message *msg){
    int res;
    /*char *str;*/
    switch(msg->return_type){
        case RPC_TY_VOID:
            switch(msg->argc){
                case 0:
                    function->fun_ptr.void_fun();
                    return_t = NULL;
                    break;
                case 1:
                    function->fun_ptr.void_fun(function->argv[0]);
                    return_t = NULL;
                    break;
                case 2:
                    function->fun_ptr.void_fun(function->argv[0], function->argv[1]);
                    return_t = NULL;
                    break;
                case 3:
                    function->fun_ptr.void_fun(function->argv[0], function->argv[1], function->argv[2]);
                    return_t = NULL;
                    break;
                default:
                    send_error(client, RPC_WRONG_NUMBER_ARGS);
            }
            break;
        case RPC_TY_INT:
            switch(msg->argc){
                case 0:
                    res = function->fun_ptr.int_fun();
                    break;
                case 1:
                    res = function->fun_ptr.int_fun(function->argv[0]);
                    break;
                case 2:
                    res = function->fun_ptr.int_fun(function->argv[0], function->argv[1]);
                    break;
                case 3:
                    res = function->fun_ptr.int_fun(function->argv[0], function->argv[1], function->argv[2]);
                    break;
                default:
                    send_error(client, RPC_WRONG_NUMBER_ARGS);
            }
            memcpy(&return_t, &res, sizeof(int));
            break;
        /*case RPC_TY_STR:
            switch(msg->argc){
                case 0:
                    return_t = &(function->fun_ptr.str_fun());
                    break;
                case 1:
                    return_t = &(function->fun_ptr.str_fun(function->argv[0]));
                    break;
                case 2:
                    return_t = &(function->fun_ptr.str_fun(function->argv[0], function->argv[1]));
                    break;
                case 3:
                    return_t = &(function->fun_ptr.str_fun(function->argv[0], function->argv[1], function->argv[2]));
                    break;
                default:
                    send_error(client, RPC_WRONG_NUMBER_ARGS);
            }
            break;*/
        default :
            send_error(client, RPC_RET_UNKNOWN_FUNC);
    }
}

void verification_function(int client, struct function_mapper *f, struct message *msg){
    int i;
    if(f->fun.argc != msg->argc)
        send_error(client, RPC_WRONG_NUMBER_ARGS);
    for(i = 0; i < f->fun.argc; i++){
        if(f->fun.argv[i] != msg->argv[i].typ)
            send_error(client, RPC_RET_WRONG_ARG);
    }
    if(f->fun.return_type != msg->return_type)
        send_error(client, RPC_RET_WRONG_TYP);
}

struct function_mapper *search_function(int client, struct message *msg){
    int ret;
    ret = exist_function(&function_memory, msg->command);
    if(ret == 0) {
        fprintf(stderr, "[%d] Unknown function `%s`.\n", getpid(), msg->command);
        send_error(client, RPC_RET_UNKNOWN_FUNC);
    }
    else if(ret == -1) {
        fprintf(stderr, "[%d] Unknown function `%s`.\n", getpid(), msg->command);
        send_error(client, RPC_RET_UNKNOWN_FUNC);
    }
    fprintf(stderr, "[%d] Function `%s` found.\n", getpid(), msg->command);
    return get_function(&function_memory, msg->command);

}

void send_error(int client, int i){
    if(send(client, &i, sizeof(int), 0) < 0)
        err(EXIT_FAILURE, "error fail to send");
    exit(EXIT_SUCCESS);
}

void read_msg(int client, struct message *msg){
    int size;
    char *from;

    if(recv(client, &size, sizeof(int), 0) < 0)
        err(EXIT_FAILURE, "Error recv\n");

    from = malloc((size+1)*sizeof(char));

    if(recv(client, from, size*sizeof(char), 0) < 0){
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

int plus(int a, int b){
    return a + b;
}

/**
 * @brief Entry point.
 * @return .
 */
int main(void)
{
    fwrite_log(stderr, "Server initialization: add some functions.");
    struct function_mapper map;
    fun_ptr_u ptr;
    ptr.int_fun = &plus;
    create_function(&map, "plus", RPC_TY_INT, ptr, 2, RPC_TY_INT, RPC_TY_INT);
    add_function(&function_memory, map);

    fwrite_log(stderr, "Server loop.");
    loop_server();
    return EXIT_SUCCESS;
}

#endif

