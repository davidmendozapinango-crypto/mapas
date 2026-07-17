#include "unity.h"
#include "graph.h"
#include "weather.h"

void run_weather_tests(void);

static Graph *build_graph(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_route(g, "R1", "A", "B", 2.0, 4.0, 6.0);
    return g;
}

static void test_base_cost_without_weather(void)
{
    Graph *g = build_graph();
    const Route *r = graph_get_edge(g, 0, 0);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, weather_effective_cost(NULL, r, MODE_WALK));
    TEST_ASSERT_EQUAL_DOUBLE(4.0, weather_effective_cost(NULL, r, MODE_BUS));
    TEST_ASSERT_EQUAL_DOUBLE(6.0, weather_effective_cost(NULL, r, MODE_CAR));
    graph_destroy(g);
}

static void test_multiplier_application(void)
{
    Graph *g = build_graph();
    WeatherMap *wm = weather_create(4);
    weather_add(wm, "R1", 2.0, 1.5, 1.0);
    const Route *r = graph_get_edge(g, 0, 0);
    TEST_ASSERT_EQUAL_DOUBLE(4.0, weather_effective_cost(wm, r, MODE_WALK));
    TEST_ASSERT_EQUAL_DOUBLE(6.0, weather_effective_cost(wm, r, MODE_BUS));
    TEST_ASSERT_EQUAL_DOUBLE(6.0, weather_effective_cost(wm, r, MODE_CAR));
    weather_destroy(wm);
    graph_destroy(g);
}

static void test_zero_multiplier_impassable(void)
{
    Graph *g = build_graph();
    WeatherMap *wm = weather_create(4);
    weather_add(wm, "R1", 0.0, 1.0, 1.0);
    const Route *r = graph_get_edge(g, 0, 0);
    TEST_ASSERT_EQUAL_DOUBLE(1e300, weather_effective_cost(wm, r, MODE_WALK));
    TEST_ASSERT_EQUAL_DOUBLE(4.0, weather_effective_cost(wm, r, MODE_BUS));
    weather_destroy(wm);
    graph_destroy(g);
}

static void test_unknown_route_returns_base_cost(void)
{
    Graph *g = build_graph();
    WeatherMap *wm = weather_create(4);
    weather_add(wm, "R2", 2.0, 2.0, 2.0);
    const Route *r = graph_get_edge(g, 0, 0);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, weather_effective_cost(wm, r, MODE_WALK));
    weather_destroy(wm);
    graph_destroy(g);
}

static void test_weather_add_duplicate_updates(void)
{
    WeatherMap *wm = weather_create(4);
    weather_add(wm, "R1", 2.0, 1.0, 1.0);
    weather_add(wm, "R1", 3.0, 1.0, 1.0);
    TEST_ASSERT_EQUAL_INT(1, wm->count);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, wm->conditions[0].multipliers[MODE_WALK]);
    weather_destroy(wm);
}

void run_weather_tests(void)
{
    RUN_TEST(test_base_cost_without_weather);
    RUN_TEST(test_multiplier_application);
    RUN_TEST(test_zero_multiplier_impassable);
    RUN_TEST(test_unknown_route_returns_base_cost);
    RUN_TEST(test_weather_add_duplicate_updates);
}
