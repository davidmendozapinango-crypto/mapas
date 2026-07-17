#include "unity.h"
#include "graph.h"
#include "dijkstra.h"
#include "weather.h"

void run_dijkstra_tests(void);

static Graph *build_graph(void)
{
    Graph *g = graph_create(8);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_place(g, "C");
    graph_add_place(g, "D");
    graph_add_route(g, "AB", "A", "B", 2.0, 2.0, 2.0);
    graph_add_route(g, "BC", "B", "C", 1.0, 1.0, 1.0);
    graph_add_route(g, "AC", "A", "C", 5.0, 5.0, 5.0);
    graph_add_route(g, "CD", "C", "D", 1.0, 1.0, 1.0);
    return g;
}

static void test_simple_path(void)
{
    Graph *g = build_graph();
    Path p = dijkstra_find(g, MODE_WALK, 0, 3, NULL, NULL, NULL, NULL, 0);
    TEST_ASSERT_EQUAL_INT(4, p.place_count);
    TEST_ASSERT_EQUAL_DOUBLE(4.0, p.total_cost);
    TEST_ASSERT_EQUAL_INT(0, p.places[0]);
    TEST_ASSERT_EQUAL_INT(3, p.places[3]);
    path_free(&p);
    graph_destroy(g);
}

static void test_no_path(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    Path p = dijkstra_find(g, MODE_WALK, 0, 1, NULL, NULL, NULL, NULL, 0);
    TEST_ASSERT_EQUAL_INT(0, p.place_count);
    TEST_ASSERT_NULL(p.places);
    path_free(&p);
    graph_destroy(g);
}

static void test_equal_cost_paths(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_place(g, "C");
    graph_add_route(g, "AB", "A", "B", 2.0, 2.0, 2.0);
    graph_add_route(g, "BC", "B", "C", 1.0, 1.0, 1.0);
    graph_add_route(g, "AC", "A", "C", 3.0, 3.0, 3.0);
    Path p = dijkstra_find(g, MODE_WALK, 0, 2, NULL, NULL, NULL, NULL, 0);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, p.total_cost);
    TEST_ASSERT_TRUE(p.place_count >= 2);
    path_free(&p);
    graph_destroy(g);
}

static void test_cycle_avoided(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_place(g, "C");
    graph_add_route(g, "AB", "A", "B", 1.0, 1.0, 1.0);
    graph_add_route(g, "BC", "B", "C", 1.0, 1.0, 1.0);
    graph_add_route(g, "CA", "C", "A", 1.0, 1.0, 1.0);
    Path p = dijkstra_find(g, MODE_WALK, 0, 2, NULL, NULL, NULL, NULL, 0);
    TEST_ASSERT_EQUAL_INT(3, p.place_count);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, p.total_cost);
    path_free(&p);
    graph_destroy(g);
}

void run_dijkstra_tests(void)
{
    RUN_TEST(test_simple_path);
    RUN_TEST(test_no_path);
    RUN_TEST(test_equal_cost_paths);
    RUN_TEST(test_cycle_avoided);
}
