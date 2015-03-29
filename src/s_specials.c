
#include "s_specials.h"

/*
 *  The current documentation.
 */
char *__doc_string = NULL;

const char *HD_NAME_STRING     = "NAME: ";
const char *HD_DOC_STRING      = "DOC: ";
const char *NO_DOC_STRING      = "- no documentation -";
const char *HD_RET_TYPE_STRING = "RETURNED TYPE: ";
const char *HD_NB_PARAM_STRING = "NUMBER OF PARAMETERS: ";
const char *HD_PARAM_TYPES     = "PARAMETERS TYPE: ";
const char *HD_TYP_STR         = "<str>";
const char *HD_TYP_INT         = "<int>";
const char *HD_TYP_VOID        = "<void>";
const char *HD_TYP_UNKNOWN     = "<unknown>";

int int_size(int i) {
    int size;

    size = 0;
    while(i != 0) {
        i /= 10;
        ++size;
    }

    return size;
}

int type_size(const char type) {
    switch (type) {
        case RPC_TY_VOID:
            return strlen(HD_TYP_VOID);
        case RPC_TY_INT:
            return strlen(HD_TYP_INT);
        case RPC_TY_STR:
            return strlen(HD_TYP_STR);
        default:
            return strlen(HD_TYP_UNKNOWN);
    }
}

const char *get_type(const char type) {
    switch (type) {
        case RPC_TY_VOID:
            return HD_TYP_VOID;
        case RPC_TY_INT:
            return HD_TYP_INT;
        case RPC_TY_STR:
            return HD_TYP_STR;
        default:
            return HD_TYP_UNKNOWN;
    }
}

int documentation_size(struct memory *memory) {
    int cpt, size;
    struct function_mapper *fmap;

    size = 0;
    for (fmap = memory->fmap.lh_first;
            fmap != NULL; fmap = fmap->fm_list.le_next) {

        /* function name, +1 for new line */
        size += strlen(HD_NAME_STRING) + fmap->name_length + 1;

        /* function description, +1 for new line */
        size += strlen(HD_DOC_STRING) + fmap->description_length + 1;

        /* ret type, +1 for new line */
        size += strlen(HD_RET_TYPE_STRING)
            + type_size(fmap->fun.return_type) + 1;

        /* number of param, +1 for new line */
        size += strlen(HD_NB_PARAM_STRING) 
            + int_size(fmap->fun.argc) + 1;

        if (fmap->fun.argc > 0) {

            /* params type */
            size += strlen(HD_PARAM_TYPES);
            cpt = 0;
            while (cpt < fmap->fun.argc) {
                size += type_size(fmap->fun.argv[cpt]);
                ++size; /* for the comma and the new line */
                ++cpt;
            }

            ++size; /* new line */
        }

    }

    /* end of function description */
    size += 1;

    return size;
}

int generate_documentation(int *size, struct memory *memory) {
    int i;
    size_t generated_size, cpt;
    struct function_mapper *fmap;

    generated_size = documentation_size(memory);

    if (size != NULL) {
        *size = generated_size;
    }

    if (generated_size <= 0) {
        return 1;
    }

    /* if an another __doc_string exists, just free it before create a new
     * one. */
    if (__doc_string != NULL) {
        free(__doc_string);
    }

    __doc_string = malloc(generated_size);
    if (__doc_string == NULL) {
        return 1;
    }

    cpt = 0;
    for (fmap = memory->fmap.lh_first;
            fmap != NULL; fmap = fmap->fm_list.le_next) {

        /* function name */
        memcpy(__doc_string + cpt, HD_NAME_STRING,
                strlen(HD_NAME_STRING));
        cpt += strlen(HD_NAME_STRING);
        memcpy(__doc_string + cpt, fmap->name, fmap->name_length);
        cpt += fmap->name_length;
        __doc_string[cpt++] = '\n';

        /* function description */
        memcpy(__doc_string + cpt, HD_DOC_STRING,
                strlen(HD_DOC_STRING));
        cpt += strlen(HD_DOC_STRING);
        if (fmap->description_length > 0) {
            memcpy(__doc_string + cpt, fmap->description,
                    fmap->description_length);
            cpt += fmap->description_length;
        } else {
            memcpy(__doc_string + cpt, NO_DOC_STRING,
                    strlen(NO_DOC_STRING));
            cpt += strlen(NO_DOC_STRING);
        }
        __doc_string[cpt++] = '\n';

        /* return type */
        memcpy(__doc_string + cpt, HD_RET_TYPE_STRING,
                strlen(HD_RET_TYPE_STRING));
        cpt += strlen(HD_RET_TYPE_STRING);
        memcpy(__doc_string + cpt, get_type(fmap->fun.return_type),
                type_size(fmap->fun.return_type));
        cpt += type_size(fmap->fun.return_type);
        __doc_string[cpt++] = '\n';

        /* number of parameters */
        memcpy(__doc_string + cpt, HD_NB_PARAM_STRING,
                strlen(HD_NB_PARAM_STRING));
        cpt += strlen(HD_NB_PARAM_STRING);
        sprintf(__doc_string + cpt, "%d", fmap->fun.argc);
        cpt += int_size(fmap->fun.argc);
        __doc_string[cpt++] = '\n';

        if (fmap->fun.argc > 0) {
            i = 0;

            memcpy(__doc_string + cpt, HD_PARAM_TYPES,
                    strlen(HD_PARAM_TYPES));
            cpt += strlen(HD_PARAM_TYPES);

            while (i < fmap->fun.argc) {
                memcpy(__doc_string + cpt,
                        get_type(fmap->fun.argv[i]),
                        type_size(fmap->fun.argv[i]));
                cpt += type_size(fmap->fun.argv[i]);

                __doc_string[cpt++] =
                    i < fmap->fun.argc - 1 ? ',' : '\n';

                ++i;
            }

            __doc_string[cpt++] = '\n';
        }
    }

    __doc_string[cpt++] = '\n';

    return 0;
}

char *get_documentation(void) {
    return __doc_string;
}

