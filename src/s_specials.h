#ifndef S_SPECIALS_H
#define S_SPECIALS_H

#include "s_mapper.h"
#include "u_rpc_data.h"

/**
 * @brief Generate the documentation of the current state of memory.
 * @param size Variable set with the size of the documentation.
 * It can be set to NULL if the size doesn't matters.
 * @param memory The current state of the memory.
 * @return 0 if everything happened correctly.
 */
int generate_documentation(int *size, struct memory *memory);

/**
 * @brief Return the documentation. IT MUST NOT BE FREED.
 * @return The pointers to the documentation.
 */
char *get_documentation(void);

#endif /* S_SPECIALS_H */

