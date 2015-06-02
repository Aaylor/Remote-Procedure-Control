
#include "s_defaults.h"

/* CHAR* FUNCTIONS */

char *documentation(void) {
    char *res;
    const char *doc;

    doc = get_documentation();
    res = malloc(strlen(doc) + 1);
    if (res == NULL) {
        return NULL;
    }

    strcpy(res, doc);
    return res;
}

char *concat(const char *s, const char *s2) {
    char *new_str;

    new_str = malloc(strlen(s) + strlen(s2) + 1);
    if (new_str == NULL) {
        return NULL;
    }

    strcpy(new_str, s);
    strcat(new_str, s2);
    return new_str;
}

char *identity(char *c) {
    char *s;
    s = malloc(strlen(c) + 1);
    if (s == NULL) {
        return NULL;
    }

    strcpy(s, c);
    return s;
}


/* INT FUNCTIONS */

int plus(int *a, int *b) {
    return *a + *b;
}

int minus(int *a, int *b) {
    return *a - *b;
}

int multiplication(int *a, int *b) {
    return *a * *b;
}

int division(int *a, int *b) {
    return *a / *b;
}

int power(int *a, int *b) {
    int cpt, res;
    if(*b == 0)
        return 1;
    cpt = *b;
    res = *a;
    while (cpt > 1) {
        res *= *a;
        --cpt;
    }
    return res;
}

int square(int *a) {
    int i = 2;
    return power(a, &i);
}

int string_length(char *s) {
    return strlen(s);
}


/* VOID FUNCTIONS */

void shutdown(const char *password) {
    if (strcmp(password, "admin") == 0) {
        kill(getppid(), SIGUSR1);
    }
}



static void add_binary_operator(struct memory *memory,
            int (*f)(int *, int *), const char *name, const char *descr) {
    struct function_mapper map;
    fun_ptr_u ptr;

    memset(&map, 0, sizeof(struct function_mapper));

    ptr.int_fun = f;
    create_function(&map, name, RPC_TY_INT, descr,
            ptr, 2, RPC_TY_INT, RPC_TY_INT);
    add_function(memory, map);
    memset(&map, 0, sizeof(struct function_mapper));
}

void add_default_functions(struct memory *memory) {
    struct function_mapper map;
    fun_ptr_u ptr;

    memset(&map, 0, sizeof(struct function_mapper));


    /*
     *  DEFAULT STRING FUNCTIONS
     *
     */

    ptr.str_fun = &documentation;
    create_function(&map, "documentation", RPC_TY_STR,
            "return the documentation of all saved functions.", ptr, 0);
    add_function(memory, map);
    memset(&map, 0, sizeof(struct function_mapper));

    ptr.str_fun = &concat;
    create_function(&map, "concat", RPC_TY_STR,
            "concatenate twe strings", ptr, 2, RPC_TY_STR, RPC_TY_STR);
    add_function(memory, map);
    memset(&map, 0, sizeof(struct function_mapper));

    ptr.str_fun = &identity;
    create_function(&map, "identity", RPC_TY_STR, "return the same str",
            ptr, 1, RPC_TY_STR);
    add_function(&function_memory, map);
    memset(&map, 0, sizeof(struct function_mapper));



    /*
     *
     *  DEFAULT VOID FUNCTIONS
     *
     */

    ptr.void_fun = &shutdown;
    create_function(&map, "shutdown", RPC_TY_VOID,
            "Shutdown the server", ptr, 1, RPC_TY_STR);
    add_function(memory, map);
    memset(&map, 0, sizeof(struct function_mapper));



    /*
     *  DEFAULT INT FUNCTIONS
     *
     */

    add_binary_operator(memory, &plus, "plus", "plus functions");
    add_binary_operator(memory, &minus, "minus", "minus functions");
    add_binary_operator(memory, &multiplication, "multiplication", 
            "multiplication functions");
    add_binary_operator(memory, &division, "division", "division functions");
    add_binary_operator(memory, &power, "power", "power functions");

    ptr.int_fun = &square;
    create_function(&map, "square", RPC_TY_INT, "square function",
            ptr, 1, RPC_TY_INT);
    add_function(&function_memory, map);
    memset(&map, 0, sizeof(struct function_mapper));

    ptr.int_fun = &string_length;
    create_function(&map, "string_length", RPC_TY_INT, "string_length function",
            ptr, 1, RPC_TY_STR);
    add_function(&function_memory, map);
    memset(&map, 0, sizeof(struct function_mapper));

}

