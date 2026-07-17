#include "output.h"
#include "dijkstra.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define INF_COST 1e300

static const char *mode_name(TransportMode mode)
{
    switch (mode) {
        case MODE_WALK: return "A pie";
        case MODE_BUS:  return "Bus";
        case MODE_CAR:  return "Carro";
        default:        return "?";
    }
}

static int route_string_length(const Graph *g, const Path *path)
{
    if (path->place_count == 0) return 22;
    int len = 0;
    for (int i = 0; i < path->place_count; i++) {
        len += (int)strlen(g->places[path->places[i]].name);
        if (i > 0) len += 4;
    }
    return len;
}

static int compute_route_width(const Graph *g, const PathResult *results)
{
    int width = 4; /* "Ruta" */
    for (int m = 0; m < MODE_COUNT; m++) {
        if (results[m].has_best) {
            int len = route_string_length(g, &results[m].best);
            if (len > width) width = len;
        }
        if (results[m].has_second) {
            int len = route_string_length(g, &results[m].second);
            if (len > width) width = len;
        }
    }
    return width < 33 ? 33 : width;
}

static void print_route(FILE *out, const Graph *g, const Path *path)
{
    if (path->place_count == 0) {
        fprintf(out, "No hay ruta disponible");
        return;
    }
    for (int i = 0; i < path->place_count; i++) {
        if (i > 0) fprintf(out, " -> ");
        fprintf(out, "%s", g->places[path->places[i]].name);
    }
}

void output_results_to_stream(FILE *out, const Graph *g, const PathResult *results)
{
    int route_width = compute_route_width(g, results);

    fprintf(out, "%-9s | %-*s | %-11s | %s\n", "Modo", route_width, "Ruta", "Costo total", "Tipo");
    fprintf(out, "----------+-");
    for (int i = 0; i < route_width; i++) fprintf(out, "-");
    fprintf(out, "-+-------------+------------\n");

    for (int m = 0; m < MODE_COUNT; m++) {
        const char *mode = mode_name((TransportMode)m);

        if (results[m].has_best) {
            char cost_str[32];
            if (results[m].best.total_cost >= INF_COST / 2.0) {
                snprintf(cost_str, sizeof(cost_str), "-");
            } else {
                snprintf(cost_str, sizeof(cost_str), "%.2f", results[m].best.total_cost);
            }
            fprintf(out, "%-9s | ", mode);
            print_route(out, g, &results[m].best);
            int pad = route_width - route_string_length(g, &results[m].best);
            for (int i = 0; i < pad; i++) fprintf(out, " ");
            fprintf(out, " | %-11s | Optima\n", cost_str);
        } else {
            fprintf(out, "%-9s | ", mode);
            print_route(out, g, &results[m].best);
            int pad = route_width - route_string_length(g, &results[m].best);
            for (int i = 0; i < pad; i++) fprintf(out, " ");
            fprintf(out, " | %-11s | Optima\n", "-");
        }

        if (results[m].has_second) {
            char cost_str[32];
            if (results[m].second.total_cost >= INF_COST / 2.0) {
                snprintf(cost_str, sizeof(cost_str), "-");
            } else {
                snprintf(cost_str, sizeof(cost_str), "%.2f", results[m].second.total_cost);
            }
            fprintf(out, "%-9s | ", mode);
            print_route(out, g, &results[m].second);
            int pad = route_width - route_string_length(g, &results[m].second);
            for (int i = 0; i < pad; i++) fprintf(out, " ");
            fprintf(out, " | %-11s | Alternativa\n", cost_str);
        } else {
            fprintf(out, "%-9s | ", mode);
            print_route(out, g, &results[m].second);
            int pad = route_width - route_string_length(g, &results[m].second);
            for (int i = 0; i < pad; i++) fprintf(out, " ");
            fprintf(out, " | %-11s | Alternativa\n", "-");
        }
    }
}

void output_results(const Graph *g, const PathResult *results)
{
    output_results_to_stream(stdout, g, results);
}
