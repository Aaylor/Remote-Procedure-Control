
#include "s_defaults.h"

/* CHAR* FUNCTIONS */

char *documentation(void) {
    printf("%s\n", get_documentation());
    return get_documentation();
}

char *identity(char *c) {
    return c;
}


/* INT FUNCTIONS */

int plus(int *a, int *b){
    return *a + *b;
}


/* VOID FUNCTIONS */




void add_default_functions(struct memory *memory) {
    struct function_mapper map;
    fun_ptr_u ptr;

    memset(&map, 0, sizeof(struct function_mapper));

    /* Documentation */
    ptr.str_fun = &documentation;
    create_function(&map, "documentation", RPC_TY_STR,
            "return the documentation of all saved functions.", ptr, 0);
    add_function(memory, map);
    memset(&map, 0, sizeof(struct function_mapper));

    /* Plus */
    ptr.int_fun = &plus;
    create_function(&map, "plus", RPC_TY_INT, "plus function",
            ptr, 2, RPC_TY_INT, RPC_TY_INT);
    add_function(&function_memory, map);
    memset(&map, 0, sizeof(struct function_mapper));

    /* Identity */
    ptr.str_fun = &identity;
    create_function(&map, "identity", RPC_TY_STR, "return the same str",
            ptr, 1, RPC_TY_STR);
    add_function(&function_memory, map);
    memset(&map, 0, sizeof(struct function_mapper));
}

