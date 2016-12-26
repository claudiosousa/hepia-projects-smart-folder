#include "../src/parser.h"
#include "vendor/cutest.h"
//#define TEST_CHECK_ //

void test_parse_empty()
{
  TEST_CHECK_(parser_parse(NULL, 0) == NULL, "should return null");
}

void test_parse_name_common(parser_t *exact_name_parser, char *file)
{
  TEST_CHECK_(exact_name_parser != NULL, "should not return null");
  TEST_CHECK_(exact_name_parser->crit == NAME, "ctril should be equal to NAME");
  TEST_CHECK_(strcmp(exact_name_parser->value, file) == 0, "value should be equal to %s", file);
}

void test_parse_name_exact()
{
  char *exact_name_argv[] = {
      "-name",
      "sample_name"};
  parser_t *exact_name_parser = parser_parse(exact_name_argv, 2);
  test_parse_name_common(exact_name_parser, exact_name_argv[1]);
  TEST_CHECK_(exact_name_parser->comp == EXACT, "comp should be equal to EXACT");
}

void test_parse_name_contains()
{
  char *exact_name_argv[] = {
      "-name",
      "-sample_name"};
  parser_t *exact_name_parser = parser_parse(exact_name_argv, 2);
  test_parse_name_common(exact_name_parser, exact_name_argv[1] + 1);
  TEST_CHECK_(exact_name_parser->comp == MIN, "comp should be equal to MIN");
}

TEST_LIST = {
    {"parse empty", test_parse_empty},
    {"parse name sample_name", test_parse_name_exact},
    {"parse name -sample_name", test_parse_name_contains},
    {"parse name -sample_name", test_parse_name_contains},
    {0}};

// void main(){
//   test_parse_name();
// }