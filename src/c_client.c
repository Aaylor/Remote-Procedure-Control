
#include "c_client.h"


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
            break;
        case RPC_TY_STR:
            if (ret != NULL) {
                strcpy(ret, ret_arg.data);
            }
            break;
        case RPC_TY_INT:
            if (ret != NULL) {
                *(int *)ret = *(int *)ret_arg.data;
            }
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

void call_documentation(void) {
    char doc[1024];

    external_call("documentation", RPC_TY_STR, doc, NULL);
    printf("%s", doc);
    exit(EXIT_SUCCESS);
}

#ifndef UNIT_TEST

int main(int argc, char **argv) {
    int cpt;

    cpt = 1;
    while (cpt < argc) {
        char *cmd = argv[cpt];

        if (strcmp(cmd, "-l") == 0 || strcmp(cmd, "--list") == 0) {
            call_documentation();
        } else if (strcmp(cmd, "-c") == 0 || strcmp(cmd, "--command") == 0) {
            /* command */
        } else {
            fprintf(stderr, "Unknown command `%s`.\n", argv[cpt]);
            exit(EXIT_FAILURE);
        }

        ++cpt;
    }
    return EXIT_SUCCESS;
}

#endif

