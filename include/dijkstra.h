#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "types.h"
#include "graph.h"
#include "weather.h"

Path dijkstra_find(const Graph *g, TransportMode mode, int src, int dst,
                   const WeatherMap *wm, const int *blocked_nodes,
                   const int *blocked_edges_from, const int *blocked_edges_to,
                   int blocked_edge_count);

void path_free(Path *p);

#endif
