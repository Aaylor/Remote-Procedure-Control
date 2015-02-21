
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

Suite *rpc_data_suite(void) {
    Suite *s;
    TCase *t;

    s = suite_create("RPC Data");
    t = tcase_create("Core");

    tcase_add_test(t, check_type_exists);
    tcase_add_test(t, check_create_message);

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

