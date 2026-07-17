#include "graph.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static int is_valid_name(const char *name) {
    if (name == NULL || name[0] == '\0') return 0;
    for (size_t i = 0; name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)name[i]) && name[i] != '_') return 0;
    }
    return 1;
}

Graph *graph_create(int initial_capacity) {
    Graph *g = malloc(sizeof(Graph));
    if (!g) return NULL;
    g->place_count = 0;
    g->place_capacity = initial_capacity > 0 ? initial_capacity : 16;
    g->places = malloc(sizeof(Place) * g->place_capacity);
    if (!g->places) {
        free(g);
        return NULL;
    }
    g->edges = malloc(sizeof(EdgeList) * g->place_capacity);
    if (!g->edges) {
        free(g->places);
        free(g);
        return NULL;
    }
    for (int i = 0; i < g->place_capacity; i++) {
        g->edges[i].routes = NULL;
        g->edges[i].count = 0;
        g->edges[i].capacity = 0;
    }
    return g;
}

void graph_destroy(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->place_count; i++) {
        free(g->places[i].name);
    }
    free(g->places);
    for (int i = 0; i < g->place_capacity; i++) {
        if (g->edges[i].routes) {
            for (int j = 0; j < g->edges[i].count; j++) {
                free(g->edges[i].routes[j].name);
            }
            free(g->edges[i].routes);
        }
    }
    free(g->edges);
    free(g);
}

static int ensure_place_capacity(Graph *g) {
    if (g->place_count < g->place_capacity) return 1;
    int new_cap = g->place_capacity * 2;
    Place *new_places = realloc(g->places, sizeof(Place) * new_cap);
    if (!new_places) return 0;
    g->places = new_places;
    EdgeList *new_edges = realloc(g->edges, sizeof(EdgeList) * new_cap);
    if (!new_edges) return 0;
    g->edges = new_edges;
    for (int i = g->place_capacity; i < new_cap; i++) {
        g->edges[i].routes = NULL;
        g->edges[i].count = 0;
        g->edges[i].capacity = 0;
    }
    g->place_capacity = new_cap;
    return 1;
}

int graph_find_place(const Graph *g, const char *name) {
    if (!g || !name) return -1;
    for (int i = 0; i < g->place_count; i++) {
        if (strcmp(g->places[i].name, name) == 0) return i;
    }
    return -1;
}

int graph_add_place(Graph *g, const char *name) {
    if (!g || !name || !is_valid_name(name)) return -1;
    int idx = graph_find_place(g, name);
    if (idx >= 0) return idx;
    if (!ensure_place_capacity(g)) return -1;
    idx = g->place_count;
    g->places[idx].name = xstrdup(name);
    if (!g->places[idx].name) return -1;
    g->places[idx].index = idx;
    g->place_count++;
    return idx;
}

int graph_add_route(Graph *g, const char *name, const char *from, const char *to,
                    double walk_cost, double bus_cost, double car_cost) {
    if (!g || !name || !from || !to) return 0;
    int from_idx = graph_find_place(g, from);
    int to_idx = graph_find_place(g, to);
    if (from_idx < 0 || to_idx < 0) return 0;

    EdgeList *list = &g->edges[from_idx];
    if (list->count >= list->capacity) {
        int new_cap = list->capacity == 0 ? 4 : list->capacity * 2;
        Route *new_routes = realloc(list->routes, sizeof(Route) * new_cap);
        if (!new_routes) return 0;
        list->routes = new_routes;
        list->capacity = new_cap;
    }

    Route *r = &list->routes[list->count];
    r->name = xstrdup(name);
    if (!r->name) return 0;
    r->from = from_idx;
    r->to = to_idx;
    r->costs[MODE_WALK] = walk_cost;
    r->costs[MODE_BUS] = bus_cost;
    r->costs[MODE_CAR] = car_cost;
    list->count++;
    return 1;
}

int graph_find_route(const Graph *g, const char *name) {
    if (!g || !name) return 0;
    for (int i = 0; i < g->place_count; i++) {
        for (int j = 0; j < g->edges[i].count; j++) {
            if (strcmp(g->edges[i].routes[j].name, name) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

const Route *graph_get_edge(const Graph *g, int from, int idx) {
    if (!g || from < 0 || from >= g->place_count) return NULL;
    if (idx < 0 || idx >= g->edges[from].count) return NULL;
    return &g->edges[from].routes[idx];
}

int graph_edge_count(const Graph *g, int from) {
    if (!g || from < 0 || from >= g->place_count) return 0;
    return g->edges[from].count;
}
