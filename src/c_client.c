

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
    int i = 0, succes = 0;
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
    if( (args = malloc(nb_args * sizeof(struct rpc_arg))) == NULL){
        fprintf(stderr, "Can't allocate memory... \n");
        return -1;
    }
    va_start(ap, ret);
    for(arg = va_arg(ap, void *); arg != NULL; arg = va_arg(ap, void *)){
        args[i].data = arg;
        args[i].typ = va_arg(ap, int);
        i++;
    }
    va_end(ap);

    /* Generate the message */
    create_message(&msg, cmd, type, nb_args, args);

    if( (clt = sendCmd(&msg)) < 0){
        succes = -1;
        goto external_call_free;
    }

    if(getAnswer(clt, type, ret) < 0)
        succes = -1;

    close(clt);

  external_call_free :
    free_message(&msg);
    free(args);

    return succes;
}


int sendCmd(struct message *msg){
    int clt, size;

    char *serial = serialize_message(&size, msg);

    if( (clt = clt_tcpsock("localhost", "23456", AF_INET))<0 ) {
        fprintf(stderr, "Unable to connect... \n");
        clt =  -1;
        goto endSendCmd;
    }

    if( send(clt, (void *)serial, size, 0) < 0) {
        fprintf(stderr, "Unable send the request... \n");
        close(clt);
        clt = -1;
    }

  endSendCmd:
    free(serial);
    return clt;
}


int getAnswer(int clt, int type, void *ret){
    int msg_size, status, succes = 0;
    char *msg;
    struct rpc_arg ret_arg;

    /* Get back the message size to read */
    if(recv(clt, &msg_size, sizeof(int), 0) < 0){
        fprintf(stderr, "Unable to receive the message size... \n");
        return -1;
    }

    if( (msg = malloc(msg_size)) == NULL){
        fprintf(stderr, "Can't allocate memory... \n");
        return -1;
    }

    /* Get back the return value */
    if(recv(clt, msg, msg_size, 0) < 0){
        fprintf(stderr, "Unable to receive the return value... \n");
        succes = -1;
        goto endGetAnswer;
    }

    status = deserialize_answer(&ret_arg, msg_size, msg);

    /* In case of error from the server */
    if(status != RPC_RET_OK){
        printErrorStatus(status);
        succes = -1;
        goto endGetAnswer;
    }

    /* Type checking */
    if(ret_arg.typ != type){
        printErrorStatus(RPC_RET_WRONG_TYP);
        succes = -1;
        goto desalocateRetArg;
    }

    switch(ret_arg.typ){
        case RPC_TY_VOID:
            ret = NULL;
        case RPC_TY_STR:
            strcpy(ret, ret_arg.data);
            break;
        case RPC_TY_INT:
            *(int *)ret = *(int *)ret_arg.data;
            break;
        default:
            assert(0); /* By deserialisation_answer */
    }

  desalocateRetArg:
    if(ret_arg.typ == RPC_TY_INT || ret_arg.typ == RPC_TY_STR)
        free(ret_arg.data);
  endGetAnswer:
    free(msg);
    return succes;
}



void printErrorStatus(int status){
    switch(status){
        case RPC_RET_OK:
            assert(0); /* By getAnswer */
            break;
        case RPC_RET_UNKNOWN_FUNC:
            fprintf(stderr, "The server doesn't knwow this function... \n");
            break;
        case RPC_RET_WRONG_ARG:
            fprintf(stderr, "The given arguments were wrong... \n");
            break;
        case RPC_RET_NO_ANSWER:
            fprintf(stderr, "The execution of the function gives no answer... \n");
            break;
        case RPC_RET_WRONG_TYP:
            fprintf(stderr, "The return type expected was wrong... \n");
            break;
        default:
            fprintf(stderr, "The server has encountered an inexpected error... \n");
            break;
    }
}

#ifndef UNIT_TEST

/**
 * @brief Entry point.
 * @return 
 */
int main(void)
{
    char str_ret[124];
    char *hello_world = "Hello World !";

    external_call("identity", RPC_TY_STR, str_ret, hello_world, RPC_TY_STR, NULL);
    printf("Get value: '%s'.\n", str_ret);

    int ret, a, b;
    a = 1;
    b = 41;

    external_call("plus", RPC_TY_INT, &ret, &a, RPC_TY_INT, &b, RPC_TY_INT, NULL);
    printf("Get value: '%d'\n", ret);

    external_call("moins", RPC_TY_INT, &ret, &a, RPC_TY_INT, &b, RPC_TY_INT, NULL);
    return EXIT_SUCCESS;
}

#endif

