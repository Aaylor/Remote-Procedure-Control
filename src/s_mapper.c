
#include "s_mapper.h"

int create_function(struct function_mapper *mapper, const char *name,
        char return_type, fun_ptr_u fun_ptr, int argc, ...) {
    int     cpt;
    size_t  fname_length;
    va_list ap;

    if (mapper == NULL) {
        return -1;
    }

    fname_length = strlen(name);
    if (fname_length > FUNCTION_NAME_LENGTH) {
        return -2;
    }

    /* FIXME: check if return_type exists */
    if (!type_exists(return_type)) {
        return -3;
    }

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

int add_function(struct memory *memory, struct function_mapper mapper) {
    if (memory == NULL) {
        return -1;
    }

    memory->fmap[memory->size] = mapper;
    ++memory->size;

    return 0;
}

int exist_function(struct memory *memory, const char *fun_name) {
    size_t cpt;

    if (memory == NULL) {
        return -1;
    }

    cpt = 0;
    while (cpt < memory->size) {
        if (strcmp(fun_name, memory->fmap[cpt].name) == 0) {
            return 1;
        }
    }

    return 0;
}

struct function_mapper *get_function(struct memory *memory,
        const char *fun_name) {
    size_t cpt;

    if (memory == NULL) {
        return NULL;
    }

    cpt = 0;
    while (cpt < memory->size) {
        if (strcmp(fun_name, memory->fmap[cpt].name) == 0) {
            return &(memory->fmap[cpt]);
        }
    }

    return NULL;
}

#ifdef DEBUG

void __print_memory_state(struct memory *memory) {
    size_t cpt;

    fprintf(stderr, "==== MEMORY STATE ====");
    if (memory == NULL) {
        fprintf(stderr, " the memory is NULL.");
    } else {
        cpt = 0;

        while (cpt < memory->size) {
            fprintf(stderr, "\n[f%zu]\n", cpt);
            __print_function_mapper_state(&(memory->fmap[cpt]));
            fprintf(stderr, "\n");
            ++cpt;
        }
    }
    fprintf(stderr, "==== END OF MEMORY STATE ====");
}

void __print_function_mapper_state(struct function_mapper *mapper) {
    if (mapper == NULL) {
        fprintf(stderr, "  mapper is null.");
    }

    fprintf(stderr, "  size of function name: %zu\n", mapper->name_length);
    fprintf(stderr, "  function name:         %s\n", mapper->name);
    __print_function_type_state(&(mapper->fun));
}

void __print_function_type_state(struct function_t *ft) {
    int cpt;

    if (ft == NULL) {
        fprintf(stderr,"    ft is null.");
    }

    fprintf(stderr, "    return type:  %d\n", ft->return_type);
    switch (ft->return_type) {
        case RPC_TY_VOID:
            fprintf(stderr, "    function ptr: %p\n", ft->fun_ptr.void_fun);
            break;
        case RPC_TY_INT:
            fprintf(stderr, "    function ptr: %p\n", ft->fun_ptr.int_fun);
            break;
        case RPC_TY_STR:
            fprintf(stderr, "    function ptr: %p\n", ft->fun_ptr.str_fun);
            break;
        default:
            fprintf(stderr, "    function ptr: invalid address");
            break;
    }
    fprintf(stderr, "    argc:         %d\n", ft->argc);

    cpt = 0;
    while (cpt < ft->argc) {
        fprintf(stderr, "     argv[%d]:    %d\n", cpt, ft->argv[cpt]);
    }
}

#endif

