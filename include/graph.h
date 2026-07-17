#ifndef GRAPH_H
#define GRAPH_H

#include "types.h"

typedef struct {
    Route *routes;
    int count;
    int capacity;
} EdgeList;

typedef struct {
    Place *places;
    int place_count;
    int place_capacity;
    EdgeList *edges;
} Graph;

Graph *graph_create(int initial_capacity);
void graph_destroy(Graph *g);
int graph_add_place(Graph *g, const char *name);
int graph_find_place(const Graph *g, const char *name);
int graph_add_route(Graph *g, const char *name, const char *from, const char *to,
                    double walk_cost, double bus_cost, double car_cost);
const Route *graph_get_edge(const Graph *g, int from, int idx);
int graph_edge_count(const Graph *g, int from);
int graph_find_route(const Graph *g, const char *name);

#endif
