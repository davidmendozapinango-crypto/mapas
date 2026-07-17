#include <stdio.h>
#include "graph.h"
#include "dijkstra.h"
#include "weather.h"

int main(void) {
    Graph *g = graph_create(4);
    graph_add_place(g, "A");
    graph_add_place(g, "B");
    graph_add_place(g, "C");
    graph_add_route(g, "AB", "A", "B", 2.0, 2.0, 2.0);
    graph_add_route(g, "BC", "B", "C", 1.0, 1.0, 1.0);
    graph_add_route(g, "AC", "A", "C", 3.0, 3.0, 3.0);
    Path p = dijkstra_find(g, MODE_WALK, 0, 2, NULL, NULL, NULL, NULL, 0);
    printf("cost=%f count=%d\n", p.total_cost, p.place_count);
    for (int i=0;i<p.place_count;i++) printf("%d ", p.places[i]);
    printf("\n");
    path_free(&p);
    graph_destroy(g);
    return 0;
}
