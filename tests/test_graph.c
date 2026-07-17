#include "unity.h"
#include "graph.h"

void run_graph_tests(void);

static void test_graph_create_and_destroy(void)
{
    Graph *g = graph_create(4);
    TEST_ASSERT_NOT_NULL(g);
    TEST_ASSERT_EQUAL_INT(0, g->place_count);
    graph_destroy(g);
}

static void test_add_place(void)
{
    Graph *g = graph_create(4);
    int a = graph_add_place(g, "A");
    int b = graph_add_place(g, "B");
    TEST_ASSERT_EQUAL_INT(0, a);
    TEST_ASSERT_EQUAL_INT(1, b);
    TEST_ASSERT_EQUAL_INT(2, g->place_count);
    graph_destroy(g);
}

static void test_duplicate_place_returns_same_index(void)
{
    Graph *g = graph_create(4);
    int a = graph_add_place(g, "A");
    int a2 = graph_add_place(g, "A");
    TEST_ASSERT_EQUAL_INT(a, a2);
    graph_destroy(g);
}

static void test_find_unknown_place(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    TEST_ASSERT_EQUAL_INT(-1, graph_find_place(g, "B"));
    graph_destroy(g);
}

static void test_directed_edge(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    TEST_ASSERT_TRUE(graph_add_route(g, "R1", "A", "B", 1.0, 2.0, 3.0));
    TEST_ASSERT_EQUAL_INT(1, graph_edge_count(g, 0));
    TEST_ASSERT_EQUAL_INT(0, graph_edge_count(g, 1));
    const Route *r = graph_get_edge(g, 0, 0);
    TEST_ASSERT_NOT_NULL(r);
    TEST_ASSERT_EQUAL_INT(1, r->to);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, r->costs[MODE_WALK]);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, r->costs[MODE_BUS]);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, r->costs[MODE_CAR]);
    graph_destroy(g);
}

static void test_multiple_edges_between_places(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_route(g, "R1", "A", "B", 1.0, 1.0, 1.0);
    graph_add_route(g, "R2", "A", "B", 2.0, 2.0, 2.0);
    TEST_ASSERT_EQUAL_INT(2, graph_edge_count(g, 0));
    graph_destroy(g);
}

static void test_find_route(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_route(g, "R1", "A", "B", 1.0, 1.0, 1.0);
    TEST_ASSERT_TRUE(graph_find_route(g, "R1"));
    TEST_ASSERT_FALSE(graph_find_route(g, "R2"));
    graph_destroy(g);
}

void run_graph_tests(void)
{
    RUN_TEST(test_graph_create_and_destroy);
    RUN_TEST(test_add_place);
    RUN_TEST(test_duplicate_place_returns_same_index);
    RUN_TEST(test_find_unknown_place);
    RUN_TEST(test_directed_edge);
    RUN_TEST(test_multiple_edges_between_places);
    RUN_TEST(test_find_route);
}
