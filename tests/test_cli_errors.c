#include "unity.h"
#include "cli.h"
#include "graph.h"
#include "weather.h"
#include <string.h>

void run_cli_errors_tests(void);

static void test_load_missing_route_file(void)
{
    Graph *g = NULL;
    char error[256];
    TEST_ASSERT_FALSE(cli_load_route_file(&g, "tests/fixtures/no_existe.txt", error, sizeof(error)));
    TEST_ASSERT_NULL(g);
    TEST_ASSERT_NOT_NULL(strstr(error, "no se pudo abrir"));
}

static void test_load_invalid_route_file(void)
{
    Graph *g = NULL;
    char error[256];
    TEST_ASSERT_FALSE(cli_load_route_file(&g, "tests/fixtures/invalid_routes.txt", error, sizeof(error)));
    TEST_ASSERT_NULL(g);
    TEST_ASSERT_NOT_NULL(strstr(error, "sintaxis invalida"));
}

static void test_validate_unknown_origin(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    char error[256];
    TEST_ASSERT_FALSE(cli_validate_origin_destination(g, "C", "B", error, sizeof(error)));
    TEST_ASSERT_NOT_NULL(strstr(error, "no es un lugar conocido"));
    graph_destroy(g);
}

static void test_validate_unknown_destination(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    char error[256];
    TEST_ASSERT_FALSE(cli_validate_origin_destination(g, "A", "C", error, sizeof(error)));
    TEST_ASSERT_NOT_NULL(strstr(error, "no es un lugar conocido"));
    graph_destroy(g);
}

static void test_validate_same_origin_destination(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    char error[256];
    TEST_ASSERT_FALSE(cli_validate_origin_destination(g, "A", "A", error, sizeof(error)));
    TEST_ASSERT_NOT_NULL(strstr(error, "diferente del origen"));
    graph_destroy(g);
}

static void test_parse_action(void)
{
    TEST_ASSERT_EQUAL_INT(0, cli_parse_action("n"));
    TEST_ASSERT_EQUAL_INT(1, cli_parse_action("C"));
    TEST_ASSERT_EQUAL_INT(2, cli_parse_action("S"));
    TEST_ASSERT_EQUAL_INT(-1, cli_parse_action("x"));
    TEST_ASSERT_EQUAL_INT(-1, cli_parse_action(""));
}

void run_cli_errors_tests(void)
{
    RUN_TEST(test_load_missing_route_file);
    RUN_TEST(test_load_invalid_route_file);
    RUN_TEST(test_validate_unknown_origin);
    RUN_TEST(test_validate_unknown_destination);
    RUN_TEST(test_validate_same_origin_destination);
    RUN_TEST(test_parse_action);
}
