
#include <stdlib.h>
#include <check.h>

#include "../src/u_rpc_data.h"

/*
 *
 *  RUNNING TEST ON U_RPC_DATA
 *
 *
 */

START_TEST (check_type_exists) {
    ck_assert_int_eq(type_exists(RPC_TY_VOID), 1);
    ck_assert_int_eq(type_exists(RPC_TY_INT),  1);
    ck_assert_int_eq(type_exists(RPC_TY_STR),  1);
    ck_assert_int_eq(type_exists(-1), 0);
    ck_assert_int_eq(type_exists(42), 0);
} END_TEST

START_TEST (check_create_message) {
    int i, ret;
    struct message msg;
    struct rpc_arg *argv;

    ret = create_message(NULL, "abc", RPC_TY_VOID, 0, NULL);
    ck_assert_int_ne(ret, 0);

    ret = create_message(&msg, NULL, RPC_TY_VOID, 0, NULL);
    ck_assert_int_ne(ret, 0);

    ret = create_message(&msg, "func", RPC_TY_VOID, 0, NULL);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(msg.command_length, strlen("func"));
    ck_assert_str_eq(msg.command, "func");
    ck_assert_int_eq(msg.argc, 0);
    ck_assert_ptr_eq(msg.argv, NULL);
    free_message(&msg);

    argv = malloc(sizeof(struct rpc_arg) * 3);
    if (argv == NULL) {
        ck_abort_msg("check_create_message: allocation memory failed.");
    }

    char *c = "message";
    argv[0].typ     = RPC_TY_STR;
    argv[0].data    = strdup(c);

    i = 42;
    argv[1].typ  = RPC_TY_INT;
    argv[1].data = malloc(sizeof(int));
    memcpy(argv[1].data, &i, sizeof(int));

    i = 24;
    argv[2].typ  = RPC_TY_INT;
    argv[2].data = malloc(sizeof(int));
    memcpy(argv[2].data, &i, sizeof(int));

    ret = create_message(&msg, "func_bis", RPC_TY_INT, 3, argv);
    free(argv[0].data); free(argv[1].data); free(argv[2].data); free(argv);
    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(msg.command_length, strlen("func_bis"));
    ck_assert_int_eq(strcmp(msg.command, "func_bis"), 0);
    ck_assert_int_eq(msg.argc, 3);
    ck_assert_ptr_ne(msg.argv, NULL);

    argv = &(msg.argv[0]);
    ck_assert_int_eq(argv->typ, RPC_TY_STR);
    ck_assert_int_eq(strlen(argv->data), strlen(c));
    ck_assert_int_eq(strcmp(argv->data, c), 0);

    argv = &msg.argv[1];
    memcpy(&i, argv->data, sizeof(int));
    ck_assert_int_eq(argv->typ, RPC_TY_INT);
    ck_assert_int_eq(i, 42);

    argv = &msg.argv[2];
    memcpy(&i, argv->data, sizeof(int));
    ck_assert_int_eq(argv->typ, RPC_TY_INT);
    ck_assert_int_eq(i, 24);

    free_message(&msg);
} END_TEST

START_TEST (check_de_serialize_integer) {
    int pos, neg, res;
    char *buffer;

    pos = 0;
    buffer = malloc(2);
    ck_assert_int_eq(serialize_integer(pos, buffer), 2);
    ck_assert_int_eq(buffer[0], 1);
    ck_assert_int_eq(buffer[1], '0');
    ck_assert_int_eq(deserialize_integer(&res, buffer), 2);
    ck_assert_int_eq(res, 0);
    free(buffer);

    pos = 42;
    buffer = malloc(3);
    ck_assert_int_eq(serialize_integer(pos, buffer), 3);
    ck_assert_int_eq(buffer[0], 2);
    ck_assert_int_eq(buffer[1], '4');
    ck_assert_int_eq(buffer[2], '2');
    ck_assert_int_eq(deserialize_integer(&res, buffer), 3);
    ck_assert_int_eq(res, 42);
    free(buffer);

    neg = -1;
    buffer = malloc(3);
    ck_assert_int_eq(serialize_integer(neg, buffer), 3);
    ck_assert_int_eq(buffer[0], 2);
    ck_assert_int_eq(buffer[1], '-');
    ck_assert_int_eq(buffer[2], '1');
    ck_assert_int_eq(deserialize_integer(&res, buffer), 3);
    ck_assert_int_eq(res, -1);
    free(buffer);

    neg = -42;
    buffer = malloc(4);
    ck_assert_int_eq(serialize_integer(neg, buffer), 4);
    ck_assert_int_eq(buffer[0], 3);
    ck_assert_int_eq(buffer[1], '-');
    ck_assert_int_eq(buffer[2], '4');
    ck_assert_int_eq(buffer[3], '2');
    ck_assert_int_eq(deserialize_integer(&res, buffer), 4);
    ck_assert_int_eq(res, -42);
    free(buffer);
} END_TEST

START_TEST (check_de_serialize_message) {
    int i, tmp, cpt;
    char *serialized;
    char buf[64];
    struct message msg;
    struct rpc_arg *argv;

    /*
     * First message
     */

    create_message(&msg, "func", RPC_TY_VOID, 0, NULL);
    serialized = serialize_message(&msg);

    /* Read the serialization */

    cpt = 0;
    ck_assert_ptr_ne(serialized, NULL);

    memcpy(&tmp, serialized, sizeof(int));
    ck_assert_int_eq(tmp, (2 * sizeof(int)) + 5);
    cpt += sizeof(int);

    memcpy(&tmp, serialized + cpt, sizeof(int));
    ck_assert_int_eq(tmp, 4);
    cpt += sizeof(int);

    memcpy(buf, serialized + cpt, 4); buf[5] = '\0';
    ck_assert_int_eq(strcmp(buf, "func"), 0);
    cpt += 4;

    ck_assert_int_eq(serialized[cpt], RPC_TY_VOID);
    cpt += 1;

    memcpy(&tmp, serialized + cpt, sizeof(int));
    ck_assert_int_eq(tmp, 0);
    cpt += sizeof(int);

    /* Deserialization */

    memcpy(&tmp, serialized, sizeof(int));
    tmp = deserialize_message(&msg, tmp, serialized + sizeof(int));
    ck_assert_int_eq(tmp, 0);
    ck_assert_int_eq(msg.command_length, strlen("func"));
    ck_assert_int_eq(strcmp(msg.command, "func"), 0);
    ck_assert_int_eq(RPC_TY_VOID, msg.return_type);
    ck_assert_int_eq(msg.argc, 0);
    ck_assert_ptr_eq(msg.argv, NULL);

    free_message(&msg);
    free(serialized);





    /*
     *  Second Message
     */

    argv = malloc(sizeof(struct rpc_arg) * 3);
    if (argv == NULL) {
        ck_abort_msg("check_create_message: allocation memory failed.");
    }

    char *c = "message";
    argv[0].typ     = RPC_TY_STR;
    argv[0].data    = strdup(c);

    i = 42;
    argv[1].typ  = RPC_TY_INT;
    argv[1].data = malloc(sizeof(int));
    memcpy(argv[1].data, &i, sizeof(int));

    i = 24;
    argv[2].typ  = RPC_TY_INT;
    argv[2].data = malloc(sizeof(int));
    memcpy(argv[2].data, &i, sizeof(int));

    create_message(&msg, "func_bis", RPC_TY_INT, 3, argv);
    serialized = serialize_message(&msg);

    memcpy(&tmp, serialized, sizeof(int));
    tmp = deserialize_message(&msg, tmp, serialized + sizeof(int));
    ck_assert_int_eq(tmp, 0);
    ck_assert_int_eq(msg.command_length, strlen("func_bis"));
    ck_assert_int_eq(strcmp(msg.command, "func_bis"), 0);
    ck_assert_int_eq(RPC_TY_INT, msg.return_type);
    ck_assert_int_eq(msg.argc, 3);
    ck_assert_ptr_ne(msg.argv, NULL);
    ck_assert_int_eq(msg.argv[0].typ, RPC_TY_STR);
    ck_assert_int_eq(strcmp(msg.argv[0].data, c), 0);
    ck_assert_int_eq(msg.argv[1].typ, RPC_TY_INT);
    ck_assert_int_eq(*(int *)msg.argv[1].data, 42);
    ck_assert_int_eq(msg.argv[2].typ, RPC_TY_INT);
    ck_assert_int_eq(*(int *)msg.argv[2].data, 24);

} END_TEST

Suite *rpc_data_suite(void) {
    Suite *s;
    TCase *t;

    s = suite_create("RPC Data");
    t = tcase_create("Core");

    tcase_add_test(t, check_type_exists);
    tcase_add_test(t, check_create_message);
    tcase_add_test(t, check_de_serialize_integer);
    tcase_add_test(t, check_de_serialize_message);

    suite_add_tcase(s, t);

    return s;
}









int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    number_failed = 0;

    /* RPC_DATA_SUITE */
    s  = rpc_data_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    number_failed += srunner_ntests_failed(sr);
    srunner_free(sr);


    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

