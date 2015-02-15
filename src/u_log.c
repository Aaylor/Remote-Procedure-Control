
#include "u_log.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/**
 * @file u_log.h
 * @brief Implements log file.
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */



int write_log(const char *msg)
{
    return dwrite_log(STDOUT_FILENO, msg);
}

int pwrite_log(const char *path, const char *msg)
{
    int fd, ret;

    fd = open(path, O_APPEND);
    if (fd == -1)
    {
        perror("pwrite_log|open()");
        return LOG_ERR;
    }

    ret = dwrite_log(fd, msg);

    if (close(fd) == -1)
    {
        perror("pwrite_log|close()");
        return LOG_ERR;
    }

    return ret;
}

int fwrite_log(FILE *file, const char *msg)
{
    int fd;

    if (file == NULL)
    {
        fprintf(stderr, "fwrite_log: file is NULL\n");
        return LOG_ERR;
    }

    fd = fileno(file);
    return dwrite_log(fd, msg);
}

int dwrite_log(int fd, const char *msg)
{
    int length;

    length  = strlen(msg);
    if (write(fd, msg, length) < length)
    {
        perror("dwrite_log|write()");
        return LOG_ERR;
    }

    return LOG_OK;
}

