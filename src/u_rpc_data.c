
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

#ifdef DEBUGLOG
    fwrite_log(stderr, "--- BEGINNING OF create_message ---");
#endif

    if (msg == NULL || command == NULL || !type_exists(return_type)) {
#ifdef DEBUGLOG
        fwrite_log(stderr, "wrong parameters.");
#endif
        return -1;
    }

    msg->command_length = strlen(command);
    msg->command = malloc(msg->command_length + 1);
    if (msg->command == NULL) {
#ifdef DEBUGLOG
        fwrite_log(stderr, "allocation has failed.");
#endif
        goto fail;
    }
    strcpy(msg->command, command);

    msg->return_type = return_type;
    msg->argc = argc;

    if (argc > 0) {
        if (argv == NULL) {
            goto fail;
        }

        msg->argv = malloc(sizeof(struct rpc_arg) * msg->argc);
        if (msg->argv == NULL) {
            goto fail;
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
                        goto fail;
                    }
                    memcpy(msg->argv[cpt].data, argv[cpt].data, sizeof(int));
                    break;

                case RPC_TY_STR:
                    msg->argv[cpt].data = malloc(strlen(argv[cpt].data) + 1);
                    if (msg->argv[cpt].data == NULL) {
                        goto fail;
                    }
                    strcpy(msg->argv[cpt].data, argv[cpt].data);
                    break;

                default:
                    break;
            }

            ++cpt;
        }
    } else {
        msg->argv = NULL;
    }

#ifdef DEBUGLOG
    fwrite_log(stderr, "-- END OF create_message,ret 0 --");
    __debug_display_message(msg);
#endif

    return 0;

fail:
    if (msg->command != NULL) {
        free(msg->command);
    }

    if (msg->argv != NULL) {
        cpt = 0;
        while (cpt < argc) {
            if (msg->argv[cpt].data != NULL) {
                free(msg->argv[cpt].data);
            }
        }
        free(msg->argv);
    }

#ifdef DEBUGLOG
    fwrite_log(stderr, "-- END OF create_message (AS FAIL) --");
#endif

    return -1;

}

void free_message(struct message *msg) {
    int cpt;

    free(msg->command);

    cpt = 0;
    while(cpt < msg->argc) {
        if (msg->argv[cpt].data != NULL) {
            free(msg->argv[cpt].data);
        }

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
        return 0;
    }

    size = cpt = 0;
    while (cpt < argc) {
        ++size;

        switch(args[cpt].typ) {
            case RPC_TY_VOID:
                break;

            case RPC_TY_STR:
                size += 1 + strlen(args[cpt].data);
                break;

            case RPC_TY_INT:
                memcpy(&tmp, args[cpt].data, sizeof(int));
                if (tmp < 0) {
                    ++size;
                    tmp = -tmp;
                }

                while(tmp != 0) {
                    tmp /= 10;
                    ++size;
                }
                ++size;
                break;

            default:
                return -1;
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

    if (i == 0) {
        buf[0] = (char)1;
        buf[1] = '0';
        return 2;
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

    neg = 0;
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

    if (neg) res *= -1;
    memcpy(result, &res, sizeof(int));

    return cpt;
}

char *serialize_message(int *msg_size, struct message *msg) {
    int  cpt, i, size, tmp;
    char *serialized_msg;
    struct rpc_arg *arg;

#ifdef DEBUGLOG
    fwrite_log(stderr, "--- BEG OF SERIALIZE_MESSAGE ---");
#endif

    if (msg == NULL) {
        return NULL;
    }

    size = (2 * sizeof(int)) + msg->command_length + 1
        + arg_size(msg->argc, msg->argv);

    serialized_msg = malloc(size + sizeof(int));
    if (serialized_msg == NULL) {
#ifdef DEBUGLOG
        fwrite_log(stderr, "allocation has failed.");
#endif
        goto fail;
    }

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
                tmp = serialize_integer(*(int *)arg->data,
                        serialized_msg + cpt);
                cpt += tmp;
                break;


            case RPC_TY_STR:
                tmp = strlen((char *)arg->data);
                memcpy(serialized_msg + cpt, &tmp, sizeof(char));
                cpt += sizeof(char);

                memcpy(serialized_msg + cpt, arg->data, tmp);
                cpt += tmp;
                break;

            default:
                goto fail;
        }

        ++i;
    }

#ifdef DEBUGLOG
    fwrite_log(stderr, "-- END OF SERIALIZE --\n");
    __debug_display_message(msg);
    __debug_display_serialized_message(serialized_msg);
#endif

    *msg_size = size + sizeof(int);
    return serialized_msg;

fail:
    if (serialized_msg != NULL) {
        free(serialized_msg);
    }

#ifdef DEBUGLOG
    fwrite_log(stderr, "-- END OF SERIALIZE (AS FAIL) --\n");
#endif

    return NULL;
}

int deserialize_message(struct message *msg, int size,
        const char *serialized_msg) {
    int cpt, i;
    char tmp;
    struct rpc_arg *arg;

#ifdef DEBUGLOG
    fwrite_log(stderr," -- BEGINNING OF DESERIALIZATION --\n");
#endif

    if (msg == NULL || serialized_msg == NULL) {
        return -1;
    }

    cpt = 0;
    memcpy(&(msg->command_length), serialized_msg + cpt, sizeof(int));
    cpt += sizeof(int);

    msg->command = malloc(msg->command_length + 1);
    if (msg->command == NULL) {
#ifdef DEBUGLOG
        fwrite_log(stderr, "\"msg->command\": allocation has failed.");
#endif
        goto fail;
    }
    memcpy(msg->command, serialized_msg + cpt, msg->command_length);
    msg->command[msg->command_length] = '\0';
    cpt += msg->command_length;

    memcpy(&(msg->return_type), serialized_msg + cpt, sizeof(char));
    cpt += sizeof(char);

    memcpy(&(msg->argc), serialized_msg + cpt, sizeof(int));
    cpt += sizeof(int);

    if (msg->argc > 0) {
        msg->argv = malloc(msg->argc * sizeof(struct rpc_arg));
        if(msg->argv == NULL) {
#ifdef DEBUGLOG
            fwrite_log(stderr, "\"msg->argv\": allocation has failed.");
#endif
            goto fail;
        }
    } else {
        msg->argv = NULL;
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
#ifdef DEBUGLOG
                    fwrite_log(stderr, "\"INT arg->data\": allocation has failed.");
#endif
                    goto fail;
                }

                cpt += deserialize_integer(arg->data, serialized_msg + cpt);
                break;

            case RPC_TY_STR:
                memcpy(&tmp, serialized_msg + cpt, sizeof(char));
                cpt += sizeof(char);

                arg->data = malloc(tmp * sizeof(char) + 1);
                if (arg->data == NULL) {
#ifdef DEBUGLOG
                    fwrite_log(stderr, "\"STR arg->data\": allocation has failed.");
#endif
                    goto fail;
                }

                memcpy(arg->data, serialized_msg + cpt, tmp);
                ((char *)arg->data)[(int)tmp] = '\0';
                cpt += tmp;
                break;

            default:
#ifdef DEBUGLOG
                fwrite_log(stderr, "arg: wrong type.");
#endif
                goto fail;
        }

        ++i;
    }

#ifdef DEBUGLOG
    dwrite_log(STDERR_FILENO, "-- END OF DESERIALIZE MESSAGE --\n");
    __debug_display_serialized_message(serialized_msg);
    __debug_display_message(msg);
#endif

    return cpt - size;

fail:
    if (msg != NULL) {
        if (msg->command != NULL) {
            free(msg->command);
        }

        if (msg->argv != NULL) {
            cpt = 0;
            while (cpt < msg->argc) {
                if (msg->argv[cpt].data != NULL) {
                    free(msg->argv[cpt].data);
                }
                ++cpt;
            }

            free(msg->argv);
        }
    }

#ifdef DEBUGLOG
    dwrite_log(STDERR_FILENO, "-- END OF DESERIALIZE MESSAGE (AS FAIL) --\n");
#endif

    return -1;
}


char *serialize_answer(int *msg_size, char status, struct rpc_arg *ret){
    int data_size, message_length, str_size;
    char *data;

    /* data_size = 0 if ret == NULL */
    if( (data_size = arg_size(1, ret)) < 0)
        return NULL;

    message_length = sizeof(char) + data_size;
    *msg_size = sizeof(int) + message_length;

    data = malloc(*msg_size);
    if (data == NULL) {
        return NULL;
    }

    if(status == RPC_RET_OK) {

        /* Case where ret can't be null */
        if(ret == NULL){
            free(data);
            return NULL;
        }

        /* Data completion */
        if(ret->typ == RPC_TY_INT){
            serialize_integer(*(int *)ret->data, data + sizeof(int) + 2);
        }
        else if(ret->typ == RPC_TY_STR){
            str_size = data_size - 2;
            memcpy(data + sizeof(int) + 3, ret->data, str_size);
            memcpy(data + sizeof(int) + 2, &str_size, sizeof(char));
        }
        memcpy(data + sizeof(int) + 1, &ret->typ, sizeof(char));

    }

    /* Return status */
    memcpy(data + sizeof(int), &status, sizeof(char));

    /* Message length */
    memcpy(data, &message_length, sizeof(int));

#ifdef DEBUGLOG
    __debug_display_serialized_answer(data);
#endif

    return data;
}



int deserialize_answer(struct rpc_arg *ret, int size,
        const char *serialized_ret){
    char status, data_length;

    if(size < 1) return -1;
    /* Get back the return status */
    memcpy(&status, serialized_ret, sizeof(char));

    if(status == RPC_RET_OK) {

        if(size < 2) return -1;
        /* Get back the return type */
        memcpy(&(ret->typ), serialized_ret + 1, sizeof(char));

        if(ret->typ == RPC_TY_INT){
            ret->data = malloc(sizeof(int));
            if(deserialize_integer(ret->data, serialized_ret + 2) < 0)
                fprintf(stderr, "Can't deserialize the integer... \n");
        }
        else if(ret->typ == RPC_TY_STR){
            /* Get back data lenght */
            memcpy(&data_length, serialized_ret + 2, sizeof(char));
            ret->data = malloc(data_length + 1);
            /* Get back the string */
            strncpy(ret->data, serialized_ret + 3, data_length);
        }
        else
            ret->data = NULL;
    }

    return status;
}



#ifdef DEBUGLOG

void __debug_display_message(struct message *msg) {
    int cpt;

    fprintf(stderr, "== DEBUGLOG: display_message ==\n");

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

    fprintf(stderr, "== DEBUGLOG: display serialized message ==\n");

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

void __debug_display_serialized_answer(const char *serialized_answer) {
    int cpt, message_length;
    const char *msg;

    fprintf(stderr, "\n\n== DEBUGLOG: display serialized answer ==\n");

    if (serialized_answer == NULL) {
        fprintf(stderr, "serialized answer is NULL.\n");
        return;
    }

    memcpy(&message_length, serialized_answer, sizeof(int));
    fprintf(stderr, "size: %d\n", message_length);

    cpt = 0;
    msg = serialized_answer + sizeof(int);
    while (cpt < message_length) {
        fprintf(stderr, "%d ", msg[cpt]);
        ++cpt;
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "== END OF serialized answer ==\n\n");
}

#endif

