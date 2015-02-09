#ifndef LOG_H
#define LOG_H

/**
 * @file log.h
 * @brief Easy way to write logs.
 * @author Tortrat-Gentilhomme Nicolas
 * @author Raymond Nicolas
 * @author Runarvot Loïc
 * @version 0.1
 * @date 2015-02-09
 */



/**
 * @brief Value returned by log functions when there is no problem.
 */
#define LOG_OK  0

/**
 * @brief Value returned by log function when there is a problem.
 */
#define LOG_ERR 1



/**
 * @brief Write the message in stdout.
 * @param msg the message to write.
 * @return LOG_OK if no error.
 */
int write_log(const char *msg);

/**
 * @brief Open the given file, write the message, and close the file.
 * @param path The log file.
 * @param msg The message.
 * @return LOG_OK if no error.
 */
int pwrite_log(const char *path, const char *msg);

/**
 * @brief Write the message in the given file.
 * @param file The log file.
 * @param msg The message.
 * @return LOG_OK if no error.
 */
int fwrite_log(FILE *file, const char *msg);

/**
 * @brief Write the message in the given file.
 * @param fd The file descriptor.
 * @param msg The message.
 * @return LOG_OK if no error.
 */
int dwrite_log(int fd, const char *msg);

#endif /* LOG_H */
