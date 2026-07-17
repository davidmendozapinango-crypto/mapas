#include "unity.h"
#include "output.h"
#include "graph.h"
#include "dijkstra.h"
#include "yen.h"
#include "weather.h"
#include <stdio.h>
#include <string.h>

void run_output_tests(void);

static FILE *open_temp(const char *name)
{
    char path[256];
    snprintf(path, sizeof(path), "tests/fixtures/%s", name);
    return fopen(path, "w+");
}

static void read_file(FILE *f, char *buf, size_t size)
{
    rewind(f);
    size_t n = fread(buf, 1, size - 1, f);
    buf[n] = '\0';
}

static void test_output_contains_header_and_modes(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_route(g, "AB", "A", "B", 1.0, 2.0, 3.0);

    PathResult results[MODE_COUNT];
    for (int m = 0; m < MODE_COUNT; m++) {
        results[m].has_best = 0;
        results[m].has_second = 0;
        yen_find_paths(g, (TransportMode)m, 0, 1, NULL, &results[m]);
    }

    FILE *f = open_temp("test_output.txt");
    TEST_ASSERT_NOT_NULL(f);
    output_results_to_stream(f, g, results);

    char buf[1024];
    read_file(f, buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Modo"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "A pie"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Bus"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Carro"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Optima"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Alternativa"));

    fclose(f);
    for (int m = 0; m < MODE_COUNT; m++) {
        path_free(&results[m].best);
        path_free(&results[m].second);
    }
    graph_destroy(g);
}

static void test_output_cost_format(void)
{
    Graph *g = graph_create(4);
    graph_add_place(g, "Origen");
    graph_add_place(g, "Destino");
    graph_add_route(g, "OD", "Origen", "Destino", 3.5, 3.5, 3.5);

    PathResult results[MODE_COUNT];
    for (int m = 0; m < MODE_COUNT; m++) {
        results[m].has_best = 0;
        results[m].has_second = 0;
        yen_find_paths(g, (TransportMode)m, 0, 1, NULL, &results[m]);
    }

    FILE *f = open_temp("test_output.txt");
    output_results_to_stream(f, g, results);

    char buf[1024];
    read_file(f, buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "3.50"));

    fclose(f);
    for (int m = 0; m < MODE_COUNT; m++) {
        path_free(&results[m].best);
        path_free(&results[m].second);
    }
    graph_destroy(g);
}

void run_output_tests(void)
{
    RUN_TEST(test_output_contains_header_and_modes);
    RUN_TEST(test_output_cost_format);
}
