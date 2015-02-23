
#include <stdlib.h>
#include <check.h>

#include "../src/s_mapper.h"
#include "../src/u_rpc_data.h"

/*
 *
 *  RUNNING TEST ON S_MAPPER
 *
 *
 *
 */

struct function_mapper function1;
struct function_mapper function2;
struct function_mapper function3;
struct function_mapper function4;

void f1(void) {
}

int f2(void) {
    return 42;
}

int f3(int i) {
    return i + 40;
}

int f4(int i, char *j, int k) {
    if (strcmp(j, "abc") == 0) {
        return i + k;
    }

    return -42;
}

START_TEST (check_create_function) {
    fun_ptr_u _f1;
    fun_ptr_u _f2;
    fun_ptr_u _f3;
    fun_ptr_u _f4;

    _f1.void_fun = &f1;
    _f2.int_fun  = &f2;
    _f3.int_fun  = &f3;
    _f4.int_fun  = &f4;

    create_function(&function1, "f1", RPC_TY_VOID, _f1, 0);
    ck_assert_int_eq(function1.name_length, strlen("f1"));
    ck_assert_int_eq(strcmp(function1.name, "f1"), 0);
    ck_assert_int_eq(function1.fun.return_type, RPC_TY_VOID);
    ck_assert_int_eq(function1.fun.argc, 0);

    create_function(&function2, "f2", RPC_TY_INT, _f2, 0);
    ck_assert_int_eq(function2.name_length, strlen("f2"));
    ck_assert_int_eq(strcmp(function2.name, "f2"), 0);
    ck_assert_int_eq(function2.fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(function2.fun.argc, 0);

    create_function(&function3, "f3", RPC_TY_INT, _f3, 1, RPC_TY_INT);
    ck_assert_int_eq(function3.name_length, strlen("f3"));
    ck_assert_int_eq(strcmp(function3.name, "f3"), 0);
    ck_assert_int_eq(function3.fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(function3.fun.argc, 1);
    ck_assert_int_eq(function3.fun.argv[0], RPC_TY_INT);

    create_function(&function4, "f4", RPC_TY_INT, _f4, 3,
            RPC_TY_INT, RPC_TY_STR, RPC_TY_INT);
    ck_assert_int_eq(function4.name_length, strlen("f4"));
    ck_assert_int_eq(strcmp(function1.name, "f4"), 0);
    ck_assert_int_eq(function4.fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(function4.fun.argc, 3);
    ck_assert_int_eq(function4.fun.argv[0], RPC_TY_INT);
    ck_assert_int_eq(function4.fun.argv[1], RPC_TY_STR);
    ck_assert_int_eq(function4.fun.argv[2], RPC_TY_INT);
} END_TEST

START_TEST (check_add_function) {
    ck_assert_int_eq(add_function(NULL, function1), -1);

    ck_assert_int_eq(add_function(&function_memory, function1), 0);
    ck_assert_int_eq(add_function(&function_memory, function2), 0);
    ck_assert_int_eq(add_function(&function_memory, function3), 0);
    ck_assert_int_eq(add_function(&function_memory, function4), 0);

    ck_assert_int_eq(function_memory.size, 4);
} END_TEST

START_TEST (check_exist_function) {
    ck_assert_int_eq(exist_function(NULL, ""), -1);
    ck_assert_int_eq(exist_function(&function_memory, "dummy"), 0);
    ck_assert_int_eq(exist_function(&function_memory, "f1"), 1);
    ck_assert_int_eq(exist_function(&function_memory, "f2"), 1);
    ck_assert_int_eq(exist_function(&function_memory, "f3"), 1);
    ck_assert_int_eq(exist_function(&function_memory, "f4"), 1);
} END_TEST

START_TEST (check_get_function) {
    struct function_mapper *fptr;

    ck_assert_ptr_eq(get_function(NULL, ""), NULL);
    ck_assert_ptr_eq(get_function(&function_memory, "dummy"), NULL);

    fptr = get_function(&function_memory, "f1");
    ck_assert_ptr_ne(fptr, NULL);
    ck_assert_int_eq(fptr->name_length, strlen("f1"));
    ck_assert_int_eq(strcmp(fptr->name, "f1"), 0);
    ck_assert_int_eq(fptr->fun.return_type, RPC_TY_VOID);
    ck_assert_int_eq(fptr->fun.argc, 0);

    fptr = get_function(&function_memory, "f2");
    ck_assert_ptr_ne(fptr, NULL);
    ck_assert_int_eq(fptr->name_length, strlen("f2"));
    ck_assert_int_eq(strcmp(fptr->name, "f2"), 0);
    ck_assert_int_eq(fptr->fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(fptr->fun.argc, 0);
    ck_assert_int_eq(fptr->fun.fun_ptr.int_fun(), 42);

    fptr = get_function(&function_memory, "f3");
    ck_assert_ptr_ne(fptr, NULL);
    ck_assert_int_eq(strcmp(fptr->name, "f3"), 0);
    ck_assert_int_eq(fptr->fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(fptr->fun.argc, 1);
    ck_assert_int_eq(fptr->fun.argv[0], RPC_TY_INT);
    ck_assert_int_eq(fptr->fun.fun_ptr.int_fun(2), 42);

    fptr = get_function(&function_memory, "f4");
    ck_assert_ptr_ne(fptr, NULL);
    ck_assert_int_eq(fptr->name_length, strlen("f4"));
    ck_assert_int_eq(strcmp(fptr->name, "f4"), 0);
    ck_assert_int_eq(fptr->fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(fptr->fun.argc, 3);
    ck_assert_int_eq(fptr->fun.argv[0], RPC_TY_INT);
    ck_assert_int_eq(fptr->fun.argv[1], RPC_TY_STR);
    ck_assert_int_eq(fptr->fun.argv[2], RPC_TY_INT);
    ck_assert_int_eq(fptr->fun.fun_ptr.int_fun(21, "abc", 21), 42);
    ck_assert_int_eq(fptr->fun.fun_ptr.int_fun(21, "dummy", 21), -42);
} END_TEST

Suite *s_mapper_suite(void) {
    Suite *s;
    TCase *t;

    s = suite_create("Server Mapper");
    t = tcase_create("Core");

    tcase_add_test(t, check_create_function);
    tcase_add_test(t, check_add_function);
    tcase_add_test(t, check_exist_function);
    tcase_add_test(t, check_get_function);

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
    s  = s_mapper_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    number_failed += srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

