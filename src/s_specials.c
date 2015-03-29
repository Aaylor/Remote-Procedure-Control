
#include "s_specials.h"

/*
 *  The current documentation.
 */
char *documentation = NULL;

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

            /* params type, +1 for new line */
            size += strlen(HD_PARAM_TYPES) + 1;
            cpt = 0;
            while (cpt < fmap->fun.argc) {
                size += type_size(fmap->fun.argv[cpt]);
                ++size; /* for the comma and the new line */
                ++cpt;
            }

            ++size; /* new line */
        }

        /* end of function description */
        size += 1;

    }

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

    /* if an another documentation exists, just free it before create a new
     * one. */
    if (documentation != NULL) {
        free(documentation);
    }

    documentation = malloc(generated_size);
    if (documentation == NULL) {
        return 1;
    }

    cpt = 0;
    for (fmap = memory->fmap.lh_first;
            fmap != NULL; fmap = fmap->fm_list.le_next) {

        /* function name */
        memcpy(documentation + cpt, HD_NAME_STRING,
                strlen(HD_NAME_STRING));
        cpt += strlen(HD_NAME_STRING);
        memcpy(documentation + cpt, fmap->name, fmap->name_length);
        cpt += fmap->name_length;
        documentation[cpt++] = '\n';

        /* function description */
        memcpy(documentation + cpt, HD_DOC_STRING,
                strlen(HD_DOC_STRING));
        cpt += strlen(HD_DOC_STRING);
        if (fmap->description_length > 0) {
            memcpy(documentation + cpt, fmap->description,
                    fmap->description_length);
            cpt += fmap->description_length;
        } else {
            memcpy(documentation + cpt, NO_DOC_STRING,
                    strlen(NO_DOC_STRING));
            cpt += strlen(NO_DOC_STRING);
        }
        documentation[cpt++] = '\n';

        /* return type */
        memcpy(documentation + cpt, HD_RET_TYPE_STRING,
                strlen(HD_RET_TYPE_STRING));
        cpt += strlen(HD_RET_TYPE_STRING);
        memcpy(documentation + cpt, get_type(fmap->fun.return_type),
                type_size(fmap->fun.return_type));
        cpt += type_size(fmap->fun.return_type);
        documentation[cpt++] = '\n';

        /* number of parameters */
        memcpy(documentation + cpt, HD_NB_PARAM_STRING,
                strlen(HD_NB_PARAM_STRING));
        cpt += strlen(HD_NB_PARAM_STRING);
        sprintf(documentation + cpt, "%d", fmap->fun.argc);
        cpt += int_size(fmap->fun.argc);
        documentation[cpt++] = '\n';

        if (fmap->fun.argc > 0) {
            i = 0;

            memcpy(documentation + cpt, HD_PARAM_TYPES,
                    strlen(HD_PARAM_TYPES));
            cpt += strlen(HD_PARAM_TYPES);

            while (i < fmap->fun.argc) {
                memcpy(documentation + cpt,
                        get_type(fmap->fun.argv[i]),
                        type_size(fmap->fun.argv[i]));
                cpt += type_size(fmap->fun.argv[i]);

                documentation[cpt++] =
                    i < fmap->fun.argc - 1 ? ',' : '\n';

                ++i;
            }

            documentation[cpt++] = '\n';
        }
    }

    documentation[cpt++] = '\n';

    return 0;
}

const char *get_documentation(void) {
    return documentation;
}

