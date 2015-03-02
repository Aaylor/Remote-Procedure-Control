

#include "c_client.h"


/**
 * @file c_client.c
 * @brief Main file to handle clients
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */

int external_call(const char *cmd, int type, void *ret, ...){
    /* Variadic variables */
    int nb_args = 0;
    va_list ap;

    /* Arguments */
    int i = 0;
    void *arg = NULL;
    struct rpc_arg *args = NULL;
    struct message msg;

    /* Socket */
    int clt = 0;

    /* Calculate the numer of arguments */
    va_start(ap, ret);
    for(arg = va_arg(ap, void *); arg != NULL; arg = va_arg(ap, void *)){
        va_arg(ap, int);
        nb_args++;
    }
    va_end(ap);

    /* Generate an argument tab */
    args = malloc(nb_args * sizeof(struct rpc_arg));
    va_start(ap, ret);
    for(arg = va_arg(ap, void *); arg != NULL; arg = va_arg(ap, void *)){
        args[i].data = arg;
        args[i].typ = va_arg(ap, int);
        i++;
    }
    va_end(ap);

    /* Generate the message */
    create_message(&msg, cmd, type, nb_args, args);

    if( (clt = sendCmd(&msg)) < 0)
        return -1;

    free_message(&msg);
    free(args);

    return 0;
}

char data_size(int data_type, void *data){
    int res = 0, t;
    char *tmp;
    switch(data_type){
        case RPC_TY_VOID:
            return 0;
        case RPC_TY_INT:
            t=10;
            while(*((int *)data)/t>0){
                res++;
                t*=10;
            }
            return res;
        case RPC_TY_STR:
            tmp = (char*)data;
            while(tmp++ != '\0')
                res++;
            return res;
        default:
            return -1;
    }
}

int sendCmd(struct message *msg){
    int clt = 0; // Client's socket filedescriptor
    char *serial = serialize_message(msg);

    if( (clt = clt_tcpsock("localhost", "23456", AF_INET))<0 ) {
        fprintf(stderr, "Unable to connect");
        return -1;
    }

    send(clt, (void *)serial, strlen(serial), 0);

    free(serial);
    return clt;
}

#ifndef UNIT_TEST

/**
 * @brief Entry point.
 * @return 
 */
int main(void)
{
    return EXIT_SUCCESS;
}

#endif

