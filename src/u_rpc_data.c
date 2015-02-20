
#include "u_rpc_data.h"


/**
 * @file u_rpc_data.c
 * @brief Implements serialize and deserialize functions.
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */


int type_exists(char type) {
    return type >= RPC_TY_VOID && type <= RPC_TY_STR;
}


int create_message(struct message *msg, const char *command, char return_type,
        int argc, struct rpc_arg *argv) {
    int cpt;

    if (msg == NULL || command == NULL || !type_exists(return_type)) {
        return -1;
    }

    msg->command_length = strlen(command);
    msg->command = strdup(command);
    if (msg->command == NULL) {
        return -1;
    }

    msg->return_type = return_type;
    msg->argc = argc;

    if (argc > 0) {
        if (argv == NULL) {
            free(msg->command);
            return -1;
        }

        msg->argv = malloc(sizeof(struct rpc_arg) * msg->argc);
        if (msg->argv == NULL) {
            free(msg->command);
            return -1;
        }

        cpt = 0;
        while (cpt < msg->argc) {
            msg->argv[cpt].typ       = argv[cpt].typ;

            switch (argv[cpt].typ) {
                case RPC_TY_VOID:
                    msg->argv[cpt].data = NULL;
                    break;

                case RPC_TY_INT:
                    msg->argv[cpt].data = malloc(sizeof(int));
                    if (msg->argv[cpt].data == NULL) {
                        free(msg->command);
                        free(msg->argv);
                        /* FIXME: possible memory leak here. */
                    }
                    memcpy(msg->argv[cpt].data, argv[cpt].data, sizeof(int));
                    break;

                case RPC_TY_STR:
                    msg->argv[cpt].data = strdup((char *)argv[cpt].data);
                    if (msg->argv[cpt].data == NULL) {
                        free(msg->command);
                        free(msg->argv);
                        /* FIXME: possible memory leak here. */
                    }
                    break;

                default:
                    break;
            }

            ++cpt;
        }
    } else {
        msg->argv = NULL;
    }

#ifdef DEBUG
    dwrite_log(STDERR_FILENO, "-- END OF create_message,ret 0 --");
    __debug_display_message(msg);
#endif

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

/**
 * @brief Returns the size of args array according to the protocol.
 * @param argc The number of argument.
 * @param args Arguments.
 * @return the size.
 */
int arg_size(int argc, struct rpc_arg *args) {
    int size, cpt, tmp;

    if (args == NULL) {
        return -1;
    }

    size = cpt = 0;
    while (cpt < argc) {

        switch(args[cpt].typ) {
            case RPC_TY_VOID:
                size += 1;
                break;

            case RPC_TY_STR:
                size += 1 + strlen((char *)args[cpt].data);
                break;

            case RPC_TY_INT:
                memcpy(&tmp, args[cpt].data, sizeof(int));
                while(tmp > 0) {
                    tmp /= 10;
                    ++size;
                }
                ++size;
                break;

            default: /* FIXME: how to handle ? */
                break;
        }

        ++cpt;
    }

    return size;
}


int serialize_integer(int i, char *buf) {
    size_t size;
    int    tmp, isize, neg;

    neg = 0;
    if (i < 0) {
        neg = 1;
        i   = -i;
    }

    size = 0;
    tmp  = i;
    while (tmp != 0) {
        ++size;
        tmp /= 10;
    }

    if (neg == 1) {
        buf[1] = '-';
        ++size;
    }

    isize = size;
    tmp   = i;
    while (tmp != 0) {
        buf[size] = '0' + (tmp % 10);
        tmp /= 10;
        --size;
    }

    buf[0] = (char)isize;

    return isize + 1;
}

int deserialize_integer(int *result, const char *msg) {
    int cpt, neg, size;
    int res;

    res = 0;

    size = (int)msg[0] + 1;

    cpt  = 1;
    if (msg[cpt] == '-') {
        neg = 1;
        ++cpt;
    }

    while (cpt < size) {
        res *= 10;
        res += msg[cpt] - '0';
        ++cpt;
    }

    memcpy(result, &res, sizeof(int));

    return cpt;
}

char *serialize_message(struct message *msg) {
    int cpt, i, size, tmp;
    char *serialized_msg;
    struct rpc_arg *arg;

    if (msg == NULL) {
        return NULL;
    }

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
                cpt += serialize_integer(*(int *)arg->data,
                        serialized_msg + cpt);
                break;


            case RPC_TY_STR:
                tmp = strlen((char *)arg->data);
                memcpy(serialized_msg + cpt, &tmp, sizeof(char));
                cpt += sizeof(char);

                memcpy(serialized_msg + cpt, arg->data, tmp);
                cpt += tmp;
                break;

            default:
                free(serialized_msg);
                return NULL;
        }

        ++i;
    }

#ifdef DEBUG
    dwrite_log(STDERR_FILENO, "-- END OF SERIALIZE --");
    __debug_display_message(msg);
    __debug_display_serialized_message(serialized_msg);
#endif

    return serialized_msg;
}

int deserialize_message(struct message *msg, const char *serialized_msg) {
    int  msg_length, cpt, i;
    char tmp;
    struct rpc_arg *arg;

    if (msg == NULL || serialized_msg == NULL) {
        return -1;
    }

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
                arg->data = NULL;
                break;

            case RPC_TY_INT:
                arg->data = malloc(sizeof(int));
                if (arg->data == NULL) {
                    free(msg->command);
                    free(msg->argv);
                    return -1;
                }

                cpt += deserialize_integer(arg->data, serialized_msg + cpt);
                break;

            case RPC_TY_STR:
                memcpy(&tmp, serialized_msg + cpt, sizeof(char));
                cpt += sizeof(char);

                arg->data = malloc(tmp * sizeof(char) + 1);
                if (arg->data == NULL) {
                    free(msg->command);
                    free(msg->argv);
                    return -1;
                }

                memcpy(arg->data, serialized_msg + cpt, tmp);
                ((char *)arg->data)[(int)tmp] = '\0';
                cpt += tmp;
                break;

            default:
                free(msg->command);
                free(msg->argv);
                return -1;
        }

        ++i;
    }

#ifdef DEBUG
    dwrite_log(STDERR_FILENO, "-- END OF DESERIALIZE MESSAGE --");
    __debug_display_serialized_message(serialized_msg);
    __debug_display_message(msg);
#endif

    return cpt - msg_length;
}


#ifdef DEBUG

void __debug_display_message(struct message *msg) {
    int cpt;

    fprintf(stderr, "== DEBUG: display_message ==\n");

    if (msg == NULL) {
        fprintf(stderr, "msg is NULL.\n");
        return;
    }

    fprintf(stderr, "%-20s: %d\n", "command_length", msg->command_length);
    fprintf(stderr, "%-20s: %s\n", "command", msg->command);
    fprintf(stderr, "%-20s: %d\n", "return_type", msg->return_type);
    fprintf(stderr, "%-20s: %d\n", "argc", msg->argc);

    cpt = 0;
    while (cpt < msg-> argc) {
        fprintf(stderr, "  argv[%d]: %d\n", cpt, msg->argv[cpt].typ);

        switch(msg->argv[cpt].typ) {
            case RPC_TY_VOID:
                fprintf(stderr, "  argv[%d]: %s\n", cpt, "NULL");
                break;

            case RPC_TY_INT:
                fprintf(stderr, "  argv[%d]: %d\n", cpt,
                        *(int *)msg->argv[cpt].data);
                break;

            case RPC_TY_STR:
                fprintf(stderr, "  argv[%d]: %s\n", cpt,
                        (char *)msg->argv[cpt].data);
                break;

            default:
                fprintf(stderr, "  argv[%d]: %s\n", cpt, "<< INTERNAL ERROR >>");
                break;
        }

        ++cpt;
    }
}

void __debug_display_serialized_message(const char *serialized_msg) {
    int cpt, message_length;
    const char *msg;

    fprintf(stderr, "== DEBUG: display serialized message ==\n");

    if (serialized_msg == NULL) {
        fprintf(stderr, "serialized_msg is NULL.\n");
        return;
    }

    memcpy(&message_length, serialized_msg, sizeof(int));
    fprintf(stderr, "size: %d\n", message_length);

    cpt = 0;
    msg = serialized_msg + sizeof(int);
    while (cpt < message_length) {
        fprintf(stderr, "%d ", msg[cpt]);
        ++cpt;
    }
    fprintf(stderr, "\n");
}

#endif

