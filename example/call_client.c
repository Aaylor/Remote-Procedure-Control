#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int p[2], r, i;
    char buf[64];
    pid_t pid;

    if (argc != 3) {
        fprintf(stderr, "%s ARG1 ARG2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *request[] = {
        "../bin/client", "-c", "concat", "-ret", "-str", "-str", argv[1],
        "-str", argv[2], NULL
    };

    printf("Request: \n  ");
    for (i = 0; i < 9; i++) {
        printf("%s ", request[i]);
    }
    printf("\n\nAnswer : \n");

    if (pipe(p) != 0) {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) { /* error */
        perror("fork()");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { /* son */
        close(p[0]);

        if (dup2(p[1], STDOUT_FILENO) == -1) {
            perror("dup2()");
            exit(EXIT_FAILURE);
        }

        if (execvp(request[0], request) == -1) {
            perror("execvp()");
            exit(EXIT_FAILURE);
        }
    }


    close(p[1]);
    while((r = read(p[0], buf, 64)) > 0) {
        write(STDOUT_FILENO, buf, r);
    }

    if (r == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }

    wait(NULL);

    return EXIT_SUCCESS;
}
