
/**
 * @file s_server.c
 * @brief Main file to handle server
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */


#include <stdlib.h>

#include "s_server.h"


int current_client = -1;

LIST_HEAD(processus_list, alive) processus_alive;


void timeout_handler(int sig) {
    fprintf(stderr, "[%d] Timeout (%d) at client %d.\n",
            getpid(), sig, current_client);
    send_error(current_client, RPC_RET_NO_ANSWER);
}

void set_timeout(int seconds) {
    fprintf(stderr, "[%d] Timeout set to %d seconds.\n", getpid(), seconds);
    struct itimerval val;

    val.it_interval.tv_sec  = 0;
    val.it_interval.tv_usec = 0;
    val.it_value.tv_sec  = seconds;
    val.it_value.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &val, NULL) < 0) {
        fprintf(stderr, "[%d] Fail to set timer...\n", getpid());
        return;
    }

    signal(SIGALRM, &timeout_handler);
}

void loop_server(void){
    struct sockaddr addr;
    socklen_t len;
    int serv, client;

    if ((serv = serv_tcpsock("23456", AF_INET)) < 0){
        /*FIXME*/
        err(EXIT_FAILURE, "Error server\n");
    }

    LIST_INIT(&processus_alive);

    len = sizeof(struct sockaddr);
    while(1){
        pid_t son;

        fwrite_log(stderr, "Waiting client.");
        if ((client=accept(serv, &addr, &len)) < 0) {
            fprintf(stderr, "[%d] Error on accept client: %s\n",
                        getpid(), strerror(errno));
            continue;
        }

        fwrite_log(stderr, "Client accepted.");
        son = fork();
        if (son < 0) { /* error */
            fwrite_log(stderr, "Fail to fork.");
            /* FIXME: send error here. */
            continue;
        }
        else if (son == 0) { /* son */
            fwrite_log(stderr, "New process: handling client.");
            gestion_client(client);
        }

        check_client_track();
        keep_track_of_client(son, client);
    }

}

void keep_track_of_client(pid_t client_pid, int client_fd) {
    struct alive *current_processus;
    current_processus = malloc(sizeof(struct alive));
    if (current_processus == NULL) {
        fprintf(stderr, "[%d] Fail to keep link with son (pid: %d).",
                    getpid(), client_pid);
    }

    current_processus->son       = client_pid;
    current_processus->client_fd = client_fd;
    LIST_INSERT_HEAD(&processus_alive, current_processus, processus_alives);
}

void check_client_track(void) {
    int res;
    struct alive *p, *next;

    p = processus_alive.lh_first;
    while(p != NULL) {
        int info, err;

        next = p->processus_alives.le_next;

        res = waitpid(p->son, &info, WNOHANG);
        if (res == 0) {
            /* Has not finished yet. */
            continue;
        } else if (res == -1) {
            /* error */
            err = errno;
            switch(err) {
                case ECHILD:
                    fprintf(stderr, "[%d] CLIENT_TRACK_SYSTEM: Unexistant client (%d).\n",
                                getpid(), p->son);
                    LIST_REMOVE(p, processus_alives);
                    close(p->client_fd);
                    free(p);
                    break;
                default:
                    fprintf(stderr, "[%d] CLIENT_TRACK_SYSTEM: Unknown error. Do nothing on pid (%d).\n",
                                getpid(), p->son);
                    break;

            }
            /* TODO: check what kind of error. */
        }

        if (WIFEXITED(info)) {
            fprintf(stderr, "[%d] CLIENT_TRACK_SYSTEM: Removing client (%d).\n",
                        getpid(), p->son);
            LIST_REMOVE(p, processus_alives);
            free(p);
        } else if (WIFSIGNALED(info)) {
            fprintf(stderr, "[%d] CLIENT_TRACK_SYSTEM: Errour found on client (%d). Killing processus and sending error message.\n",
                        getpid(), p->son);

            LIST_REMOVE(p, processus_alives);
            send_error(p->client_fd, RPC_RET_NO_ANSWER);
            close(p->client_fd);
            free(p);
        } else {
            /* Should not appear... */
        }

        p = next;
        continue;
    }
}

void gestion_client(int client){
    current_client = client;
    set_timeout(5);
    execute_client(client);
}

void execute_client(int client){
    struct message msg;
    struct function_mapper *function;
    int size;
    char *ret=NULL;

    fwrite_log(stderr, "Reading client message.");
    read_msg(client, &msg);

    fwrite_log(stderr, "Search the function queried.");
    function = search_function(client, &msg);

    fwrite_log(stderr, "Function verification.");
    verification_function(client, function, &msg);

    fwrite_log(stderr, "Function execution.");
    execute_function(client, &ret, &size, &function->fun, &msg);

    fwrite_log(stderr, "Send answer to client.");
    send_answer(client, ret, size);

    free(ret);
    close(client);
    exit(EXIT_SUCCESS);
}

void send_answer(int client, char *ret, int size){
    if(send(client, ret, size, 0) < 0) {
        fprintf(stderr, "[%d] Error on sending answer. Shutdown the service. (%s)\n",
                getpid(), strerror(errno));
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
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

void send_error(int client, char c){
    int size;
    char *msg;
    msg = serialize_answer(&size, c, NULL);
    if(send(client, msg, size, 0) < 0)
        err(EXIT_FAILURE, "error fail to send");
    exit(EXIT_SUCCESS);
}

void read_msg(int client, struct message *msg){
    int size;
    char *from;

    if(recv(client, &size, sizeof(int), 0) < 0) {
        fprintf(stderr, "[%d] %s (%s)\n", getpid(),
                    "Error on receiving the message size.",
                    strerror(errno));
        exit(EXIT_FAILURE);
    }

#ifdef DEBUGLOG
    fprintf(stderr, "[%d] Message length: %d.\n", getpid(), size);
#endif

    from = malloc((size+1)*sizeof(char));

    if(recv(client, from, size*sizeof(char), 0) < 0) {
        free(from);
        fprintf(stderr, "[%d] %s (%s)\n", getpid(),
                    "Error on receiving the message.",
                    strerror(errno));
        exit(EXIT_FAILURE);
    }

#ifdef DEBUGLOG
    fwrite_log(stderr, "Message received.");
    __debug_display_serialized_message(from);
#endif

    if(deserialize_message(msg, size, from) == -1) {
        fprintf(stderr, "[%d] %s\n", getpid(),
                    "Error on deserialize the message. Killing the process.");
        exit(EXIT_FAILURE);
    }

    free(from);
}

#ifndef UNIT_TEST

int plus(int *a, int *b){
    return *a + *b;
}

char *identity(char *c) {
    return c;
}

/**
 * @brief Entry point.
 * @return .
 */
int main(void)
{
    fwrite_log(stderr, "Server initialization: add some functions.");
    struct function_mapper map, map2;
    fun_ptr_u ptr, ptr2;
    ptr.int_fun = &plus;
    create_function(&map, "plus", RPC_TY_INT, "plus function",
            ptr, 2, RPC_TY_INT, RPC_TY_INT);
    add_function(&function_memory, map);

    ptr2.str_fun = &identity;
    create_function(&map2, "identity", RPC_TY_STR, "return the same str",
            ptr2, 1, RPC_TY_STR);
    add_function(&function_memory, map2);

    fwrite_log(stderr, "Server loop.");
    loop_server();
    return EXIT_SUCCESS;
}

#endif

