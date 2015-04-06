
#include <stdlib.h>
#include <check.h>

#include <sys/queue.h>

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

void init_f1() {
    fun_ptr_u _f1;
    _f1.void_fun = &f1;
    create_function(&function1, "f1", RPC_TY_VOID, "function 1", _f1, 0);
}

void init_f2() {
    fun_ptr_u _f2;
    _f2.int_fun  = &f2;
    create_function(&function2, "f2", RPC_TY_INT, "function 2", _f2, 0);
}

void init_f3() {
    fun_ptr_u _f3;
    _f3.int_fun  = &f3;
    create_function(&function3, "f3", RPC_TY_INT, "function 3", _f3, 1,
            RPC_TY_INT);
}

void init_f4() {
    fun_ptr_u _f4;
    _f4.int_fun  = &f4;
    create_function(&function4, "f4", RPC_TY_INT, "function 4", _f4, 3,
            RPC_TY_INT, RPC_TY_STR, RPC_TY_INT);
}

void init_all() {
    init_f1();
    init_f2();
    init_f3();
    init_f4();
}

void add_all() {
    ck_assert_int_eq(add_function(&function_memory, function1), 0);
    ck_assert_int_eq(add_function(&function_memory, function2), 0);
    ck_assert_int_eq(add_function(&function_memory, function3), 0);
    ck_assert_int_eq(add_function(&function_memory, function4), 0);
}

START_TEST (check_create_function) {
    init_f1();
    ck_assert_int_eq(function1.name_length, strlen("f1"));
    ck_assert_int_eq(strcmp(function1.name, "f1"), 0);
    ck_assert_int_eq(function1.fun.return_type, RPC_TY_VOID);
    ck_assert_int_eq(strcmp(function1.description, "function 1"), 0);
    ck_assert_int_eq(strlen("function 1"), function1.description_length);
    ck_assert_int_eq(function1.fun.argc, 0);

    init_f2();
    ck_assert_int_eq(function2.name_length, strlen("f2"));
    ck_assert_int_eq(strcmp(function2.name, "f2"), 0);
    ck_assert_int_eq(function2.fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(strcmp(function2.description, "function 2"), 0);
    ck_assert_int_eq(strlen("function 2"), function2.description_length);
    ck_assert_int_eq(function2.fun.argc, 0);

    init_f3();
    ck_assert_int_eq(function3.name_length, strlen("f3"));
    ck_assert_int_eq(strcmp(function3.name, "f3"), 0);
    ck_assert_int_eq(function3.fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(strcmp(function3.description, "function 3"), 0);
    ck_assert_int_eq(strlen("function 3"), function3.description_length);
    ck_assert_int_eq(function3.fun.argc, 1);
    ck_assert_int_eq(function3.fun.argv[0], RPC_TY_INT);

    init_f4();
    ck_assert_int_eq(function4.name_length, strlen("f4"));
    ck_assert_int_eq(strcmp(function4.name, "f4"), 0);
    ck_assert_int_eq(function4.fun.return_type, RPC_TY_INT);
    ck_assert_int_eq(strcmp(function4.description, "function 4"), 0);
    ck_assert_int_eq(strlen("function 4"), function4.description_length);
    ck_assert_int_eq(function4.fun.argc, 3);
    ck_assert_int_eq(function4.fun.argv[0], RPC_TY_INT);
    ck_assert_int_eq(function4.fun.argv[1], RPC_TY_STR);
    ck_assert_int_eq(function4.fun.argv[2], RPC_TY_INT);
} END_TEST

START_TEST (check_add_function) {
    struct function_mapper *fmap;

    init_all();

    ck_assert_int_eq(add_function(NULL, function1), -1);

    ck_assert_int_eq(add_function(&function_memory, function1), 0);
    ck_assert_int_eq(add_function(&function_memory, function2), 0);
    ck_assert_int_eq(add_function(&function_memory, function3), 0);
    ck_assert_int_eq(add_function(&function_memory, function4), 0);


    ck_assert_int_eq(function_memory.size, 4);

    fmap = LIST_FIRST(&function_memory.fmap);
    ck_assert_ptr_ne(fmap, NULL);
    ck_assert_int_eq(strcmp(fmap->name, "f4"), 0);

    fmap = LIST_NEXT(fmap, fm_list);
    ck_assert_ptr_ne(fmap, NULL);
    ck_assert_int_eq(strcmp(fmap->name, "f3"), 0);

    fmap = LIST_NEXT(fmap, fm_list);
    ck_assert_ptr_ne(fmap, NULL);
    ck_assert_int_eq(strcmp(fmap->name, "f2"), 0);

    fmap = LIST_NEXT(fmap, fm_list);
    ck_assert_ptr_ne(fmap, NULL);
    ck_assert_int_eq(strcmp(fmap->name, "f1"), 0);

    fmap = LIST_NEXT(fmap, fm_list);
    ck_assert_ptr_eq(fmap, NULL);
} END_TEST

START_TEST (check_exist_function) {
    init_all();
    add_all();

    ck_assert_int_eq(exist_function(NULL, ""), -1);
    ck_assert_int_eq(exist_function(&function_memory, "dummy"), 0);
    ck_assert_int_eq(exist_function(&function_memory, "f1"), 1);
    ck_assert_int_eq(exist_function(&function_memory, "f2"), 1);
    ck_assert_int_eq(exist_function(&function_memory, "f3"), 1);
    ck_assert_int_eq(exist_function(&function_memory, "f4"), 1);
} END_TEST

START_TEST (check_get_function) {
    struct function_mapper *fptr;

    init_all();
    add_all();

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

START_TEST(check_remove_function) {
    init_all();
    add_all();

    ck_assert_int_eq(remove_function(&function_memory, "f3"), 0);
    ck_assert_int_eq(function_memory.size, 3);
    ck_assert_int_eq(remove_function(&function_memory, "f5"), -1);
} END_TEST

START_TEST(check_clear_function) {
    init_all();
    add_all();

    clear_mapper(&function_memory);
    ck_assert_int_eq(function_memory.size, 0);
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
    tcase_add_test(t, check_remove_function);
    tcase_add_test(t, check_clear_function);

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

