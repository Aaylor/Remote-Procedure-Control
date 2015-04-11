
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

int clt_sock(char *sock_path){
    struct sockaddr_un addr;
    int clt;

    if((clt = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "[%d] %s (%s)\n", getpid(),
                    "Error when initialising the client socket.",
                    strerror(errno));
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (connect(clt, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) < 0){
        fprintf(stderr, "[%d] %s (%s)\n", getpid(),
                    "Error when connecting the client socket.",
                    strerror(errno));
        return -1;
    }

    return clt;
}

int sendCmd(struct message *msg){
    int clt, size;

    char *serial = serialize_message(&size, msg);

    if( (clt = clt_sock(SOCK_PATH))<0 ) {
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

    external_call("documentation", RPC_TY_STR, doc, NULL, NULL);
    printf("%s", doc);
    exit(EXIT_SUCCESS);
}

void call_shutdown(char *password) {
    external_call("shutdown", RPC_TY_VOID, NULL, password, RPC_TY_STR, NULL);
}

#ifndef UNIT_TEST

#define STR_EQ(str1, str2) strcmp(str1, str2) == 0

int main(int argc, char **argv) {
    int cpt;

    cpt = 1;
    while (cpt < argc) {
        char *cmd = argv[cpt];

        if (STR_EQ(cmd, "-l") || STR_EQ(cmd, "--list")) {
            call_documentation();
        } else if (STR_EQ(cmd, "-s") || STR_EQ(cmd, "--shutdown")) {
            if (++cpt >= argc) {
                fprintf(stderr, "Expected password for shutdown command.\n");
                exit(EXIT_FAILURE);
            }
            call_shutdown(argv[cpt]);
        } else if (STR_EQ(cmd, "-c") || STR_EQ(cmd, "--command")) {
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

