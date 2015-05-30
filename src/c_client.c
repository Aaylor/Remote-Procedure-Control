
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

    if( transmition(ret, &msg) < 0 )
        succes = -1;

    free_message(&msg);
    free(args);

    return succes;
}

int transmition(void *ret, struct message *msg){
    int clt = 0, succes = 0;

    if( (clt = sendCmd(msg)) < 0){
        return -1;
    }

    if(getAnswer(clt, msg->return_type, ret) < 0)
        succes = -1;

    close(clt);

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
        perror("Unable send the request... \n");
        close(clt);
        clt = -1;
    }

  endSendCmd:
    if (serial != NULL)
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
            if (ret != NULL && ret_arg.data != NULL) {
                strcpy(ret, ret_arg.data);
            }
            break;
        case RPC_TY_INT:
            if (ret != NULL && ret_arg.data != NULL) {
                *(int *)ret = *(int *)ret_arg.data;
            }
            break;
        default:
            assert(0); /* By deserialisation_answer */
    }

  desalocateRetArg:
    if((ret_arg.typ == RPC_TY_INT || ret_arg.typ == RPC_TY_STR)
            && ret_arg.data != NULL) {
        free(ret_arg.data);
    }

  endGetAnswer:
    if (msg != NULL)
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
        case RPC_WRONG_NUMBER_ARGS:
            fprintf(stderr, "The number of arguments is wrong... \n");
        case RPC_RET_NO_ANSWER:
            fprintf(stderr, "The execution of the function gives no answer... \n");
            break;
        case RPC_RET_WRONG_TYP:
            fprintf(stderr, "The return type expected was wrong... \n");
            break;
        default:
            fprintf(stderr, "The client has encountered an inexpected error... \n");
            break;
    }
}

void call_documentation(void) {
    char doc[2048];

    if (external_call("documentation", RPC_TY_STR, doc, NULL, NULL) == -1) {
        exit(EXIT_FAILURE);
    }

    printf("%s", doc);

    exit(EXIT_SUCCESS);
}

void call_shutdown(char *password) {
    external_call("shutdown", RPC_TY_VOID, NULL, password, RPC_TY_STR, NULL);
}

#ifndef UNIT_TEST

#define STR_EQ(str1, str2) strcmp(str1, str2) == 0

static inline int is_type_command(const char *cmd) {
    return STR_EQ(cmd, "-int") || STR_EQ(cmd, "-str") || STR_EQ(cmd, "-void");
}

char returned_type(const char *cmd) {
    if (STR_EQ(cmd, "-int"))
        return RPC_TY_INT;
    else if (STR_EQ(cmd, "-str"))
        return RPC_TY_STR;
    else
        return RPC_TY_VOID;
}

void command_line_error(int pos) {
    fprintf(stderr, "Error while parsing command line, at %d.\n", pos);
    fprintf(stderr, "Should be like: ");
    fprintf(stderr, "--command function_name -ret -typ [-typ arg1 ... -typ argN]\n");
    fprintf(stderr, "Where typ could be: -int, -str, -void.\n");
    exit(EXIT_FAILURE);
}

int check_command(int argc, char **argv, int current_cpt) {
    int cpt, nb_arg;

    /* Arg form: --command function_name -ret -typ -typ arg1 ... -typ argN */
    if (current_cpt + 2 >= argc)
        command_line_error(argc);

    nb_arg = 0;
    for (cpt = current_cpt + 3; cpt < argc; cpt += 2) {
        char ret;

        if (!is_type_command(argv[cpt])) {
            fprintf(stderr, "Argument type (%s) is not correct.\n", argv[cpt]);
            command_line_error(cpt);
        }

        ret = returned_type(argv[cpt]);

        if (ret != RPC_TY_VOID && cpt + 1 >= argc)
            command_line_error(cpt + 1);

        nb_arg++;
    }

    return nb_arg;
}

void parse_command(struct message *msg, int argc, char **argv, int current_cpt) {
    int cpt, arg_cpt, nb_arg;
    struct rpc_arg *arg;

    nb_arg = check_command(argc, argv, current_cpt);

    arg = NULL;
    if (nb_arg != 0) {
        arg = malloc(nb_arg * sizeof(struct rpc_arg));
        if (arg == NULL) {
            perror("malloc()");
            exit(EXIT_FAILURE);
        }
    }

    msg->command_length = strlen(argv[current_cpt]);
    msg->command = malloc(msg->command_length);
    if (msg->command == NULL) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
    strcpy(msg->command, argv[current_cpt]);
    msg->return_type = returned_type(argv[current_cpt + 2]);

    arg_cpt = 0;
    for(cpt = current_cpt + 3; cpt < argc; cpt += 2) {
        int integer_result;

        arg[arg_cpt].typ = returned_type(argv[cpt]);
        switch (arg[arg_cpt].typ) {
            case RPC_TY_INT:
                integer_result = atoi(argv[cpt + 1]);
                arg[arg_cpt].data = malloc(sizeof(int));
                if (arg[arg_cpt].data == NULL) {
                    perror("malloc()");
                    exit(EXIT_FAILURE);
                }

                memcpy(arg[arg_cpt].data, &integer_result, sizeof(int));
                break;


            case RPC_TY_STR:
                arg[arg_cpt].data = malloc(strlen(argv[cpt + 1]));
                if (arg[arg_cpt].data == NULL) {
                    perror("malloc()");
                    exit(EXIT_FAILURE);
                }

                strcpy(arg[arg_cpt].data, argv[cpt + 1]);

                break;

            case RPC_TY_VOID:
                fprintf(stderr, "Void is not allowed as parameter type.\n");
                exit(EXIT_FAILURE);

            default:
                fprintf(stderr, "UNKNOWN TYPE.\n");
        }

        ++arg_cpt;
    }

    msg->argc = nb_arg;
    msg->argv = arg;
}

int main(int argc, char **argv) {
    int cpt;
    struct message msg;
    void *ret;

#ifdef DEBUGLOG
    int __debug_i;

    fprintf(stderr, "[ARGV PRINTING]\n");
    for (__debug_i = 0; __debug_i < argc; __debug_i++) {
        fprintf(stderr, "%2d: %s\n", __debug_i, argv[__debug_i]);
    }
    fprintf(stderr, "[ARGV PRINTING]\n\n");
#endif

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
            parse_command(&msg, argc, argv, cpt + 1);

#ifdef DEBUGLOG
            fprintf(stderr, "command option generated following message:\n");
            __debug_display_message(&msg);
#endif

            switch(msg.return_type) {
                case RPC_TY_INT:
                    ret = malloc(sizeof(int));
                    if (ret == NULL) {
                        perror("malloc()");
                        exit(EXIT_FAILURE);
                    }
                    break;
                case RPC_TY_STR:
                    ret = malloc(256); /* FIXME */
                    if (ret == NULL) {
                        perror("malloc()");
                        exit(EXIT_FAILURE);
                    }
                    break;
                case RPC_TY_VOID:
                    ret = NULL;
                    break;
                default:
                    fprintf(stderr, "Return type failed.\n");
                    exit(EXIT_FAILURE);
            }

            if( transmition(ret, &msg) <0 )
                exit(EXIT_FAILURE);

            switch(msg.return_type){
                case RPC_TY_INT:
                    printf("%d", *(int *)ret);
                    break;
                case RPC_TY_STR:
                    printf("%s", (char *)ret);
                    break;
                case RPC_TY_VOID:
                    printf("Execution succes !\n");
                    break;
            }

            if (ret != NULL)
                free(ret);

            free_message(&msg);
            break; /* parse_command take the whole command line */
        } else {
            fprintf(stderr, "Unknown command `%s`.\n", argv[cpt]);
            exit(EXIT_FAILURE);
        }

        ++cpt;
    }
    return EXIT_SUCCESS;
}

#endif

