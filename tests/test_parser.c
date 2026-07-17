#include "unity.h"
#include "parser.h"
#include "graph.h"
#include "weather.h"
#include <string.h>

void run_parser_tests(void);

static void test_parse_valid_route_file(void)
{
    Graph *g = graph_create(8);
    char error[256];
    TEST_ASSERT_TRUE(parse_route_file("tests/fixtures/valid_routes.txt", g, error, sizeof(error)));
    TEST_ASSERT_EQUAL_INT(4, g->place_count);
    TEST_ASSERT_EQUAL_INT(0, graph_find_place(g, "A"));
    TEST_ASSERT_EQUAL_INT(1, graph_find_place(g, "B"));
    graph_destroy(g);
}

static void test_parse_missing_route_file(void)
{
    Graph *g = graph_create(8);
    char error[256];
    TEST_ASSERT_FALSE(parse_route_file("tests/fixtures/no_existe.txt", g, error, sizeof(error)));
    TEST_ASSERT_NOT_NULL(strstr(error, "no se pudo abrir"));
    graph_destroy(g);
}

static void test_parse_invalid_route_syntax(void)
{
    Graph *g = graph_create(8);
    char error[256];
    TEST_ASSERT_FALSE(parse_route_file("tests/fixtures/invalid_routes.txt", g, error, sizeof(error)));
    TEST_ASSERT_NOT_NULL(strstr(error, "sintaxis invalida"));
    graph_destroy(g);
}

static void test_parse_duplicate_route_ignored(void)
{
    Graph *g = graph_create(8);
    char error[256];
    TEST_ASSERT_TRUE(parse_route_file("tests/fixtures/duplicate_routes.txt", g, error, sizeof(error)));
    TEST_ASSERT_EQUAL_INT(1, graph_edge_count(g, 0));
    const Route *r = graph_get_edge(g, 0, 0);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, r->costs[MODE_WALK]);
    graph_destroy(g);
}

static void test_parse_valid_weather_file(void)
{
    WeatherMap *wm = weather_create(8);
    char error[256];
    TEST_ASSERT_TRUE(parse_weather_file("tests/fixtures/valid_weather.txt", wm, error, sizeof(error)));
    TEST_ASSERT_EQUAL_INT(2, wm->count);
    weather_destroy(wm);
}

static void test_parse_invalid_weather_skipped(void)
{
    WeatherMap *wm = weather_create(8);
    char error[256];
    TEST_ASSERT_TRUE(parse_weather_file("tests/fixtures/invalid_weather.txt", wm, error, sizeof(error)));
    /* The first line is invalid, the second line has valid numbers but references unknown route. */
    TEST_ASSERT_EQUAL_INT(1, wm->count);
    weather_destroy(wm);
}

void run_parser_tests(void)
{
    RUN_TEST(test_parse_valid_route_file);
    RUN_TEST(test_parse_missing_route_file);
    RUN_TEST(test_parse_invalid_route_syntax);
    RUN_TEST(test_parse_duplicate_route_ignored);
    RUN_TEST(test_parse_valid_weather_file);
    RUN_TEST(test_parse_invalid_weather_skipped);
}
