#include "unity.h"
#include "graph.h"
#include "yen.h"
#include "dijkstra.h"
#include "weather.h"

void run_yen_tests(void);

static Graph *build_graph(void)
{
    Graph *g = graph_create(8);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_place(g, "C");
    graph_add_place(g, "D");
    graph_add_route(g, "AB", "A", "B", 1.0, 1.0, 1.0);
    graph_add_route(g, "BC", "B", "C", 1.0, 1.0, 1.0);
    graph_add_route(g, "CD", "C", "D", 1.0, 1.0, 1.0);
    graph_add_route(g, "AD", "A", "D", 5.0, 5.0, 5.0);
    graph_add_route(g, "BD", "B", "D", 5.0, 5.0, 5.0);
    return g;
}

static void test_yen_finds_best_and_second(void)
{
    Graph *g = build_graph();
    PathResult r;
    yen_find_paths(g, MODE_WALK, 0, 3, NULL, &r);
    TEST_ASSERT_TRUE(r.has_best);
    TEST_ASSERT_TRUE(r.has_second);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, r.best.total_cost);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, r.second.total_cost);
    path_free(&r.best);
    path_free(&r.second);
    graph_destroy(g);
}

static void test_yen_no_second_path(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_route(g, "AB", "A", "B", 1.0, 1.0, 1.0);
    PathResult r;
    yen_find_paths(g, MODE_WALK, 0, 1, NULL, &r);
    TEST_ASSERT_TRUE(r.has_best);
    TEST_ASSERT_FALSE(r.has_second);
    path_free(&r.best);
    graph_destroy(g);
}

static void test_yen_no_path(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    PathResult r;
    yen_find_paths(g, MODE_WALK, 0, 1, NULL, &r);
    TEST_ASSERT_FALSE(r.has_best);
    TEST_ASSERT_FALSE(r.has_second);
    graph_destroy(g);
}

void run_yen_tests(void)
{
    RUN_TEST(test_yen_finds_best_and_second);
    RUN_TEST(test_yen_no_second_path);
    RUN_TEST(test_yen_no_path);
}
