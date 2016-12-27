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
    char *test_argv[] = {"-group", "some_group"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == GROUP, "crit token is %d should be %d", parser->crit, GROUP);
    TEST_CHECK_(strcmp(parser->value, test_argv[1]) == 0, "value should be equal to %s", test_argv[1]);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");
}

void test_parse_user() {
    char *test_argv[] = {"-user", "some_user"};
    parser_t *parser = parser_parse(test_argv, 2);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == USER, "crit token is %d should be %d", parser->crit, USER);
    TEST_CHECK_(strcmp(parser->value, test_argv[1]) == 0, "value should be equal to %s", test_argv[1]);
    TEST_CHECK_(parser->comp == EXACT, "comp should be equal to EXACT");
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

void test_simple_composition() {
    char *test_argv[] = {"-name", "test", "-and", "-size", "20"};
    parser_t *parser = parser_parse(test_argv, 5);
    TEST_CHECK_(parser != NULL, "should not return null");
    TEST_CHECK_(parser->crit == AND, "1st token is %d should be %d", parser->crit, AND);
    TEST_CHECK_(parser->next->crit == SIZE, "2nd token is %d and should be %d", parser->next->crit, SIZE);
    TEST_CHECK_(parser->next->next->crit == NAME, "3rd token is %d and should be should be %d",
                parser->next->next->crit, NAME);
    TEST_CHECK_(parser->next->next->next == NULL, "4rd token should be NULL");
}

TEST_LIST = {{"parse empty", test_parse_empty}, {"parse incomplete exp", test_parse_incomplete},
             {"parse incorrect exp", test_parse_incorrect}, {"parse name", test_parse_name},
             {"parse name contains", test_parse_name_contains}, {"parse group", test_parse_group},
             {"parse user", test_parse_user}, {"parse permission", test_parse_perm},
             {"parse perm. contains", test_parse_perm_contains}, {"parse wrong permission", test_parse_wrong_perm},
             {"parse size", test_parse_size}, {"parse wrong size", test_parse_wrong_size},
             {"parse atime", test_parse_atime}, {"parse ctime", test_parse_ctime}, {"parse mtime", test_parse_mtime},
             {"parse wrong time", test_parse_wrong_time}, {"parse operators", test_parse_operators},
             {"parse parenthesis", test_parse_parenthesis},
             {"parse wrong parenthesis", test_parse_wrong_parenthesis},
             {"simple composition", test_simple_composition},
             {0}};
