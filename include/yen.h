#ifndef YEN_H
#define YEN_H

#include "types.h"
#include "graph.h"
#include "weather.h"

int yen_find_paths(const Graph *g, TransportMode mode, int src, int dst,
                   const WeatherMap *wm, PathResult *result);

#endif
