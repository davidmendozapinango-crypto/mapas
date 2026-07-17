#ifndef OUTPUT_H
#define OUTPUT_H

#include "types.h"
#include "graph.h"
#include <stdio.h>

void output_results(const Graph *g, const PathResult *results);
void output_results_to_stream(FILE *out, const Graph *g, const PathResult *results);

#endif
