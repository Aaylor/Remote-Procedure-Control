
#include "s_mapper.h"

int create_function(struct function_mapper *mapper, const char *name,
        char return_type, union fun_ptr_u fun_ptr, int argc, ...) {
    int     cpt;
    size_t  fname_length;
    va_list ap;

    if (mapper == NULL) {
        return -1;
    }

    fname_length = strlen(name);
    if (fname_length > FUNCTION_NAME_LENGTH) {
        return -1;
    }

    /* FIXME: check if return_type exists */

    mapper->name_length = fname_length;
    strcpy(mapper->name, name);

    mapper->fun.return_type = return_type;
    mapper->fun.argc        = argc;

    cpt = 0;
    va_start(ap, argc);
    while (cpt < argc) {
        mapper->fun.argv[cpt] = (char)va_arg(ap, int);
        ++cpt;
    }
    va_end(ap);

    mapper->fun.fun_ptr     = fun_ptr;

    return 0;
}

void add_function(struct memory *memory, struct function_mapper mapper) {
    memory->fmap[memory->size] = mapper;
    ++memory->size;
}

