
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rpc_data.h"


/**
 * @file rpc_data.c
 * @brief Implements serialize and deserialize functions.
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */



/**
 * @brief Return the number of characters to write the number as string.
 * @param number the number.
 * @return how many character it needs.
 */
int get_int_characters(int number)
{
    int cpt;

    cpt = 0;
    do
    {
        cpt++;
        number /= 10;
    } while (number > 0);

    return cpt;
}

char *serialize_data(const struct rpc_arg *arg)
{
    return tserialize_data(arg->typ, arg->data);
}

char *tserialize_data(unsigned int typ, void *data)
{
    char *msg;
    char len;

    switch (typ)
    {
        case RPC_TY_VOID:
            msg = malloc(sizeof(char));

            if (msg == NULL)
                break;

            msg[0] = RPC_TY_VOID;
            break;

        case RPC_TY_INT:
            len = get_int_characters(*(int *)data);
            msg = malloc(len * sizeof(char) + 2);

            if (msg == NULL)
                break;

            msg[0] = RPC_TY_INT;
            msg[1] = len;
            sprintf(msg + 2, "%d", *(int *) data);
            break;

        case RPC_TY_STR:
            len = strlen((char *)data);
            msg = malloc((len * sizeof(char)) + 2);

            if (msg == NULL)
                break;

            msg[0] = RPC_TY_STR;
            msg[1] = len;
            strcpy(msg + 2, (char *)data);
            break;

        default:
            return NULL;
    }

    if (msg == NULL && errno != 0)
    {
        perror("tserialize_data|malloc()");
        return NULL;
    }

    return msg;
}

