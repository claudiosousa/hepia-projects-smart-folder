/** This files performs unit testing on the parser module.

    Are unit tested:
     - empty, wrong and incomplete expressions
     - individual criteria
     - implicit/explicit operators
     - boolean operators priority
     - parentheses forced priority
     - combinations of all above

    /!\ attention: to keep the code as concice and readable as possible, allocated memory is not freed
*/

#include <math.h>
#include "../src/parser.h"
#include "vendor/cutest.h"

void test_parse_empty() {
    TEST_CHECK_(parser_parse(NULL, 0) == NULL, "should return null");
}

void test_parse_incomplete() {
    char *test_argv[] = {"-name"};
    TEST_CHECK_(parser_parse(test_argv, 1) == NULL, "should return null");
}

void test_parse_incorrect() {
    char *test_argv[] = {"-wrong_arg"};
    TEST_CHECK_(parser_parse(test_argv, 1) == NULL, "should return null");
}

void test_parse_name() {
    char *test_argv[] = {"-name", "some_name"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == NAME, "crit token is %d should be %d", parser->crit, NAME);
    TEST_CHECK_(strcmp(parser->value, test_argv[1]) == 0, "value should be equal to %s", test_argv[1]);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");
}

void test_parse_name_contains() {
    char *test_argv[] = {"-name", "-some_name"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(strcmp(parser->value, test_argv[1] + 1) == 0, "value should be equal to %s", test_argv[1] + 1);
    TEST_CHECK_(parser->comp == MIN, "comp should be equal to MIN");
}

void test_parse_group() {
    char *test_argv[] = {"-group", "root"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == GROUP, "crit token is %d should be %d", parser->crit, GROUP);
    TEST_CHECK_(*(int *)parser->value == 0, "value is %d and should be 0", *(int *)parser->value);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");
}

void test_parse_wrong_group() {
    char *test_argv[] = {"-group", "unknown_group"};
    TEST_CHECK_(parser_parse(test_argv, 2) == NULL, "should return null");
}

void test_parse_user() {
    char *test_argv[] = {"-user", "root"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == USER, "crit token is %d should be %d", parser->crit, USER);
    TEST_CHECK_(*(int *)parser->value == 0, "value is %d and should be 0", *(int *)parser->value);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");
}

void test_parse_wrong_user() {
    char *test_argv[] = {"-user", "unknown_user"};
    TEST_CHECK_(parser_parse(test_argv, 2) == NULL, "should return null");
}

void test_parse_perm() {
    char *test_argv[] = {"-perm", "776"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == PERM, "crit token is %d should be %d", parser->crit, PERM);
    TEST_CHECK_(*(int *)parser->value == 0776, "value is %d and should be %d", *(int *)parser->value, 0776);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");

    test_argv[1] = "022";
    parser = parser_parse(test_argv, 2);
    TEST_CHECK_(*(int *)parser->value == 022, "value is %d and should be %d", *(int *)parser->value, 022);
}

void test_parse_perm_contains() {
    char *test_argv[] = {"-perm", "-224"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == PERM, "crit token is %d should be %d", parser->crit, PERM);
    TEST_CHECK_(*(int *)parser->value == 0224, "value is %d and should be %d", *(int *)parser->value, 0224);
    TEST_CHECK_(parser->comp == MIN, "comp should be equal to MIN");
}

void test_parse_wrong_perm() {
    char *test_argv[] = {"-perm", "1234"};
    TEST_CHECK_(parser_parse(test_argv, 2) == NULL, "should return null");
    test_argv[1] = "-1234";
    TEST_CHECK_(parser_parse(test_argv, 2) == NULL, "should return null");
}

void test_parse_size() {
    long K_BINARY = powl(2, 10);

    char *test_argv[] = {"-size", "200"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == SIZE, "crit token is %d should be %d", parser->crit, SIZE);
    TEST_CHECK_(*(long *)parser->value == 200, "value is %ld and should be %ld", *(long *)parser->value, 200);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");

    test_argv[1] = "-10c";
    parser = parser_parse(test_argv, 2);
    TEST_CHECK_(*(long *)parser->value == 10, "value is %ld and should be %ld", *(long *)parser->value, 10);
    TEST_CHECK_(parser->comp == MIN, "comp should be equal to MIN");

    test_argv[1] = "+2k";
    parser = parser_parse(test_argv, 2);
    TEST_CHECK_(*(long *)parser->value == 2 * K_BINARY, "value is %ld and should be %ld", *(long *)parser->value,
                2 * K_BINARY);
    TEST_CHECK_(parser->comp == MAX, "comp should be equal to MAX");

    test_argv[1] = "+123M";
    parser = parser_parse(test_argv, 2);
    TEST_CHECK_(*(long *)parser->value == 123 * K_BINARY * K_BINARY, "value is %ld and should be %ld",
                *(long *)parser->value, 123 * K_BINARY * K_BINARY);
    TEST_CHECK_(parser->comp == MAX, "comp should be equal to MAX");

    test_argv[1] = "-5G";
    parser = parser_parse(test_argv, 2);
    TEST_CHECK_(*(long *)parser->value == 5 * K_BINARY * K_BINARY * K_BINARY, "value is %ld and should be %ld",
                *(long *)parser->value, 5 * K_BINARY * K_BINARY * K_BINARY);
    TEST_CHECK_(parser->comp == MIN, "comp should be equal to MIN");
}

void test_parse_wrong_size() {
    char *test_argv[] = {"-size", "20v"};
    TEST_CHECK_(parser_parse(test_argv, 2) == NULL, "should return null");

    test_argv[1] = "G";
    TEST_CHECK_(parser_parse(test_argv, 2) == NULL, "should return null");
}

void test_parse_atime() {
    char *test_argv[] = {"-atime", "-20"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == ATIME, "crit token is %d should be %d", parser->crit, ATIME);
    TEST_CHECK_(*(long *)parser->value == 20, "value is %d and should be %d", *(long *)parser->value, 20);
    TEST_CHECK_(parser->comp == MIN, "comp should be equal to MIN");
}

void test_parse_ctime() {
    char *test_argv[] = {"-ctime", "+3d"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == CTIME, "crit token is %d should be %d", parser->crit, CTIME);
    TEST_CHECK_(*(long *)parser->value == 3 * 24 * 60 * 60, "value is %d and should be %d", *(long *)parser->value,
                3 * 24 * 60 * 60);
    TEST_CHECK_(parser->comp == MAX, "comp should be equal to MAX");
}

void test_parse_mtime() {
    char *test_argv[] = {"-mtime", "300m"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == MTIME, "crit token is %d should be %d", parser->crit, MTIME);
    TEST_CHECK_(*(long *)parser->value == 300 * 60, "value is %d and should be %d", *(long *)parser->value, 300 * 60);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");
}

void test_parse_wrong_time() {
    char *test_argv[] = {"-mtime", "300x"};
    TEST_CHECK_(parser_parse(test_argv, 2) == NULL, "should return null");

    test_argv[1] = "m";
    TEST_CHECK_(parser_parse(test_argv, 2) == NULL, "should return null");
}

void test_parse_operators() {
    char *test_argv[] = {"-and"};
    parser_t *parser = parser_parse(test_argv, 1);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == AND, "crit token is %d should be %d", parser->crit, AND);

    test_argv[0] = "-or";
    parser = parser_parse(test_argv, 1);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == OR, "crit token is %d should be %d", parser->crit, OR);

    test_argv[0] = "-not";
    parser = parser_parse(test_argv, 1);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == NOT, "crit token is %d should be %d", parser->crit, NOT);
}

void test_parse_parenthesis() {
    char *test_argv[] = {"(", "-name", "test", ")"};
    parser_t *parser = parser_parse(test_argv, 4);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == NAME, "crit token is %d should be %d", parser->crit, NAME);
    TEST_CHECK_(strcmp(parser->value, test_argv[2]) == 0, "value is %s and should be %s", parser->value, test_argv[2]);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");
    TEST_CHECK_(parser->next == NULL, "next token should be equal to NULL");
}

void test_parse_wrong_parenthesis() {
    char *test_argv[] = {"(", "-name", "test"};
    TEST_CHECK_(parser_parse(test_argv, 3) == NULL, "should return null");

    test_argv[0] = ")";
    TEST_CHECK_(parser_parse(test_argv, 1) == NULL, "should return null");

    test_argv[0] = "(";
    TEST_CHECK_(parser_parse(test_argv, 1) == NULL, "should return null");
}

void test_order(char *args, parser_crit_t *expected) {
    int MAX_ARGV_COUNT = 100;
    char *argv[MAX_ARGV_COUNT];
    char *args_cp = strdup(args);
    char *token;
    int argc = 0;
    while ((token = strsep(&args_cp, " "))) argv[argc++] = token;

    free(args_cp);

    parser_t *parser = parser_parse(argv, argc);
    int i = 0;
    while (expected[i] != 0) {
        TEST_CHECK_(parser != NULL, "parser %d should not be null", i);
        TEST_CHECK_(parser->crit == expected[i], "token %d is %d should be %d", i, parser->crit, expected[i]);
        parser = parser->next;
        i++;
    }

    TEST_CHECK_(parser == NULL, "token %d should be NULL", i);
}

void test_not_name() {
    // NOT(NAME)
    test_order("-not -name test", (parser_crit_t[]){NOT, NAME, 0});
}

void test_name_or_size() {
    // OR(SIZE, NAME)
    test_order("-name test -or -size 20", (parser_crit_t[]){OR, SIZE, NAME, 0});
}

void test_not_name_or_size() {
    // OR(SIZE, NOT(NAME))
    test_order("-not -name test -or -size 20", (parser_crit_t[]){OR, SIZE, NOT, NAME, 0});
}

void test_name_or_not_size() {
    // OR(NOT(SIZE), NAME)
    test_order("-name test -or -not -size 20", (parser_crit_t[]){OR, NOT, SIZE, NAME, 0});
}

void test_not_name_or_size_p() {
    // NOT(OR(SIZE, NAME))
    test_order("-not ( -name test -or -size 20 )", (parser_crit_t[]){NOT, OR, SIZE, NAME, 0});
}

void test_name_size() {
    // AND(SIZE, NAME)
    test_order("-name test -size 20", (parser_crit_t[]){AND, SIZE, NAME, 0});
}

void test_name_not_size() {
    // AND(NOT(SIZE), NAME)
    test_order("-name test -not -size 20", (parser_crit_t[]){AND, NOT, SIZE, NAME, 0});
}

void test_name_and_size() {
    // AND(SIZE, NAME)
    test_order("-name test -and -size 20", (parser_crit_t[]){AND, SIZE, NAME, 0});
}

void test_user_and_size_or_name() {
    // OR(NAME, AND(SIZE, USER))
    test_order("-user root -and -size 20 -or -name test", (parser_crit_t[]){OR, NAME, AND, SIZE, USER, 0});
}

void test_user_and_size_or_name_p() {
    //     // AND(OR(NAME, SIZE), USER))
    //     test_order("-user root -and ( -size 20 -or -name test )", (parser_crit_t[]){AND, OR, NAME, SIZE, USER, 0});
}

void test_name_or_size_and_user() {
    // OR(AND(USER, SIZE), NAME)
    test_order("-name test -or -size 20 -and -user root", (parser_crit_t[]){OR, AND, USER, SIZE, NAME, 0});
}

void test_name_or_size_and_user_p() {
    // OR(AND(USER, SIZE), NAME)
    test_order("( -name test -or -size 20 ) -and -user root", (parser_crit_t[]){AND, USER, OR, SIZE, NAME, 0});
}

void test_group_size_not_user_or_perm() {
    // OR(OR(PERM, NOT(USER)), AND(SIZE, GROUP))
    test_order("-group root -and -size 20 -or -not -user root -or -perm 777",
               (parser_crit_t[]){OR, OR, PERM, NOT, USER, AND, SIZE, GROUP, 0});
}

void test_group_not_size_or_user_perm() {
    // OR(AND(PERM, USER), AND(NOT(SIZE)), GROUP))
    test_order("-group root -not -size 20 -or -user root -perm 777",
               (parser_crit_t[]){OR, AND, PERM, USER, AND, NOT, SIZE, GROUP, 0});
}

void test_group_not_size_or_user_perm_p() {
    // AND(AND(PERM, NOT(OR(USER, SIZE)), GROUP)
    test_order("-group root -not ( -size 20 -or -user root ) -perm 777",
               (parser_crit_t[]){AND, AND, PERM, NOT, OR, USER, SIZE, GROUP, 0});
}

// List of tests to be performed
TEST_LIST = {{"parse empty", test_parse_empty},
             {"parse incomplete exp", test_parse_incomplete},
             {"parse incorrect exp", test_parse_incorrect},
             {"parse name", test_parse_name},
             {"parse name contains", test_parse_name_contains},
             {"parse group", test_parse_group},
             {"parse wrong group", test_parse_wrong_group},
             {"parse user", test_parse_user},
             {"parse wrong user", test_parse_wrong_user},
             {"parse permission", test_parse_perm},
             {"parse perm. contains", test_parse_perm_contains},
             {"parse wrong permission", test_parse_wrong_perm},
             {"parse size", test_parse_size},
             {"parse wrong size", test_parse_wrong_size},
             {"parse atime", test_parse_atime},
             {"parse ctime", test_parse_ctime},
             {"parse mtime", test_parse_mtime},
             {"parse wrong time", test_parse_wrong_time},
             {"parse operators", test_parse_operators},
             {"parse parenthesis", test_parse_parenthesis},
             {"parse wrong parenthesis", test_parse_wrong_parenthesis},
             {"-not -name test", test_not_name},
             {"-name test -or -size 20", test_name_or_size},
             {"-not -name test -or -size 20", test_not_name_or_size},
             {"-name test -or -not -size 20", test_name_or_not_size},
             {"-not ( -name test -or -size 20 )", test_not_name_or_size_p},
             {"-name test -size 20", test_name_size},
             {"-name test -not -size 20", test_name_not_size},
             {"-name test -and -size 20", test_name_and_size},
             {"-user root -and -size 20 -or -name test", test_user_and_size_or_name},
             {"-user root -and ( -size 20 -or -name test )", test_user_and_size_or_name_p},
             {"-name test -or -size 20 -and -user root", test_name_or_size_and_user},
             {"( -name test -or -size 20 ) -and -user root", test_name_or_size_and_user_p},
             {"-group root -and -size 20 -or -not -user root -or -perm 777", test_group_size_not_user_or_perm},
             {"-group root -not -size 20 -or -user root -perm 777", test_group_not_size_or_user_perm},
             {"-group root -not ( -size 20 -or -user root ) -perm 777", test_group_not_size_or_user_perm_p},
             {0}};
