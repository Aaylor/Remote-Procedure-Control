
#include "u_rpc_data.h"


/**
 * @file rpc_data.c
 * @brief Implements serialize and deserialize functions.
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */

int create_message(struct message *msg, char *command, char return_type,
        int argc, struct rpc_arg *argv) {
    int cpt;

    msg->command_length = strlen(command);
    msg->command = strdup(command);
    if (msg->command == NULL) {
        return -1;
    }

    msg->return_type = return_type;
    msg->argc = argc;

    if (argc > 0) {
        msg->argv = malloc(sizeof(struct rpc_arg) * msg->argc);
        if (msg->argv == NULL) {
            free(msg->command);
            return -1;
        }

        cpt = 0;
        while (cpt < msg->argc) {
            msg->argv[cpt].typ       = argv[cpt].typ;
            msg->argv[cpt].data_size = argv[cpt].data_size;

            msg->argv[cpt].data = malloc(msg->argv[cpt].data_size);
            if (msg->argv[cpt].data == NULL) {
                free(msg->command);
                free(msg->argv);
                /* FIXME: possible memory leak here */
            }
            memcpy(msg->argv[cpt].data, argv[cpt].data, argv[cpt].data_size);

            ++cpt;
        }
    } else {
        msg->argv = NULL;
    }

    return 0;
}


void free_message(struct message *msg) {
    int cpt;

    free(msg->command);

    cpt = 0;
    while(cpt < msg->argc) {
        free(msg->argv[cpt].data);
        ++cpt;
    }

    free(msg->argv);
}


int arg_size(int argc, struct rpc_arg *args) {
    int size, cpt;

    size = cpt = 0;
    while (cpt < argc)
    {
        switch(args[cpt].typ)
        {
            case RPC_TY_VOID:
                size += sizeof(char);
                break;
            case RPC_TY_STR:
            case RPC_TY_INT:
                size += 2 * sizeof(char) + args[cpt].data_size;
                break;
            default: /* FIXME: how to handle ? */
                break;
        }

        ++cpt;
    }

    return size;
}

char *serialize_integer(int i) {
    size_t size;
    char   *msg;
    int    tmp;

    size = 0;
    tmp  = i;
    while (tmp > 0) {
        ++size;
        tmp /= 10;
    }

    msg = malloc(size);
    if (msg == NULL) {
        return NULL;
    }
    --size;

    tmp  = i;
    while (tmp > 0) {
        msg[size] = '0' + (tmp % 10);
        tmp /= 10;
        --size;
    }

    return msg;
}

char *serialize_message(struct message *msg) {
    int cpt, i, size;
    char *serialized_msg;
    struct rpc_arg *arg;

    size = 2 * sizeof(int) + msg->command_length + 1 + msg->argc
        + arg_size(msg->argc, msg->argv);

    serialized_msg = malloc(size);
    if (serialized_msg == NULL)
        return NULL;

    cpt = 0;
    memcpy(serialized_msg + cpt, &size, sizeof(int));
    cpt += sizeof(int);

    memcpy(serialized_msg + cpt,  &(msg->command_length), sizeof(int));
    cpt += sizeof(int);

    memcpy(serialized_msg + cpt, msg->command, msg->command_length);
    cpt += msg->command_length;

    memcpy(serialized_msg + cpt, &(msg->return_type), sizeof(char));
    cpt += sizeof(char);

    memcpy(serialized_msg + cpt, &(msg->argc), sizeof(int));
    cpt += sizeof(int);

    i = 0;
    while (i < msg->argc)
    {
        arg = &msg->argv[i];

        memcpy(serialized_msg + cpt, &(arg->typ), sizeof(char));
        cpt += sizeof(char);

        switch(msg->argv[i].typ)
        {
            case RPC_TY_VOID:
                break;

            case RPC_TY_INT:
            case RPC_TY_STR:
                memcpy(serialized_msg + cpt, &(arg->data_size), sizeof(char));
                cpt += sizeof(char);

                memcpy(serialized_msg + cpt, arg->data, arg->data_size);
                cpt += arg->data_size;
                break;

            default:
                free(serialized_msg);
                return NULL;
        }

        ++i;
    }

    return serialized_msg;
}

int deserialize_message(struct message *msg, const char *serialized_msg) {
    int msg_length, cpt, i;
    struct rpc_arg *arg;

    cpt = 0;

    memcpy(&msg_length, serialized_msg, sizeof(int));
    cpt += sizeof(int);

    memcpy(&(msg->command_length), serialized_msg + cpt, sizeof(int));
    cpt += sizeof(int);

    msg->command = malloc(msg->command_length + 1);
    if (msg->command == NULL) {
        return -1;
    }
    msg->command[msg->command_length] = '\0';
    memcpy(msg->command, serialized_msg + cpt, msg->command_length);
    cpt += msg->command_length;

    memcpy(&(msg->return_type), serialized_msg + cpt, sizeof(char));
    cpt += sizeof(char);

    memcpy(&(msg->argc), serialized_msg + cpt, sizeof(int));
    cpt += sizeof(int);

    msg->argv = malloc(msg->argc * sizeof(struct rpc_arg));
    if(msg->argv == NULL) {
        free(msg->command);
        return -1;
    }

    i = 0;
    while (i < msg->argc) {
        arg = &(msg->argv[i]);

        memcpy(&(arg->typ), serialized_msg + cpt, sizeof(char));
        cpt += sizeof(char);

        switch (arg->typ) {
            case RPC_TY_VOID:
                arg->data_size = 0;
                arg->data      = NULL;
                break;

            case RPC_TY_INT:
            case RPC_TY_STR:
                memcpy(&(arg->data_size), serialized_msg + cpt, sizeof(char));
                cpt += sizeof(char);

                arg->data = malloc(arg->data_size * sizeof(char));
                if (arg->data == NULL) {
                    free(msg->command);
                    free(msg->argv);
                    return -1;
                }

                memcpy(arg->data, serialized_msg + cpt, arg->data_size);
                cpt += arg->data_size;
                break;

            default:
                free(msg->command);
                free(msg->argv);
                return -1;
        }

        ++i;
    }

    return cpt - msg_length;
}
/*
 *
 *int main(void) {
 *    int cpt;
 *    struct message m, answer;
 *    struct rpc_arg *argv;
 *
 *    m.command_length = 10;
 *    m.command        = "abcdefghij";
 *    m.return_type    = RPC_TY_INT;
 *    m.argc           = 2;
 *
 *    argv = malloc(sizeof(struct rpc_arg) * 2);
 *
 *    argv[0].typ = RPC_TY_INT;
 *    argv[0].data_size = 3;
 *    argv[0].data = serialize_integer(123);
 *
 *    argv[1].typ = RPC_TY_STR;
 *    argv[1].data_size = 5;
 *    argv[1].data = "xyza";
 *
 *    m.argv = argv;
 *
 *    char *msg = serialize_message(&m);
 *    deserialize_message(&answer, msg);
 *
 *    printf("m.command_length: %d\n", answer.command_length);
 *    printf("m.command:        %s\n", answer.command);
 *    printf("m.return_type:    %d\n", answer.return_type);
 *    printf("m.argc:           %d\n", answer.argc);
 *
 *    cpt = 0;
 *    while (cpt < answer.argc) {
 *        printf(" m.argv[%d]:      %d\n", cpt, answer.argv[cpt].typ);
 *        printf(" m.argv[%d]:      %d\n", cpt, answer.argv[cpt].data_size);
 *        printf(" m.argv[%d]:      %s\n", cpt, answer.argv[cpt].data);
 *        ++cpt;
 *    }
 *
 *    return EXIT_SUCCESS;
 *}
 *
 */

