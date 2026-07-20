#include "output.h"
#include "dijkstra.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

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

static const char *mode_color(TransportMode mode)
{
    switch (mode) {
        case MODE_WALK: return "\x1b[32m"; /* green */
        case MODE_BUS:  return "\x1b[34m"; /* blue */
        case MODE_CAR:  return "\x1b[35m"; /* magenta */
        default:        return "\x1b[0m";
    }
}


static int route_string_length(const Graph *g, const Path *path)
{
    if (path->place_count == 0) return 22;
    int len = 0;
    for (int i = 0; i < path->place_count; i++) {
        len += (int)strlen(g->places[path->places[i]].name);
        /* separator between nodes: ' -> ' (4 chars) */
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

static void print_padded_colored(FILE *out, const char *s, int width, const char *color, int use_color, int right_align)
{
    /* prepare a padded visible string first to keep alignment correct when using ANSI colors */
    char buf[2048];
    if (right_align)
        snprintf(buf, sizeof(buf), "%*s", width, s);
    else
        snprintf(buf, sizeof(buf), "%-*s", width, s);
    if (use_color && color) {
        const char *reset = "\x1b[0m";
        fprintf(out, "%s%s%s", color, buf, reset);
    } else {
        fprintf(out, "%s", buf);
    }
}

void output_results_to_stream(FILE *out, const Graph *g, const PathResult *results)
{
    int route_width = compute_route_width(g, results);
    int use_color = 0;
#ifndef _WIN32
    /* prefer STDOUT_FILENO check to avoid implicit declaration of fileno() */
    if (out == stdout) {
        use_color = isatty(STDOUT_FILENO);
    } else if (out == stderr) {
        use_color = isatty(STDERR_FILENO);
    } else {
        use_color = 0;
    }
#endif
    /* allow overriding colors via env vars: MAPAS_COLOR=1 to force, MAPAS_NO_COLOR or NO_COLOR to disable */
    const char *env_no = getenv("MAPAS_NO_COLOR");
    if (!env_no) env_no = getenv("NO_COLOR");
    const char *env_yes = getenv("MAPAS_COLOR");
    if (env_no) use_color = 0;
    else if (env_yes) use_color = (*env_yes != '0');

    /* use_color will be used below to optionally wrap cells in ANSI codes */

    /* compute actual column widths based on content */
    int mode_w = 0;
    int cost_w = (int)strlen("Costo total");
    int type_w = (int)strlen("Alternativa");
    for (int m = 0; m < MODE_COUNT; m++) {
        int ml = (int)strlen(mode_name((TransportMode)m)) + 3; /* prefix >> and space */
        if (ml > mode_w) mode_w = ml;
        if (results[m].has_best) {
            char buf[64]; snprintf(buf, sizeof(buf), "%.2f", results[m].best.total_cost);
            int l = (int)strlen(buf); if (l > cost_w) cost_w = l;
        }
        if (results[m].has_second) {
            char buf[64]; snprintf(buf, sizeof(buf), "%.2f", results[m].second.total_cost);
            int l = (int)strlen(buf); if (l > cost_w) cost_w = l;
        }
    }
    if (mode_w < 6) mode_w = 6;
    int cell_mode = mode_w;
    int cell_route = route_width;
    int cell_cost = cost_w;
    int cell_type = type_w;

    /* top border */
    fprintf(out, "+");
    for (int i = 0; i < cell_mode + 2; i++) fputc('-', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_route + 2; i++) fputc('-', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_cost + 2; i++) fputc('-', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_type + 2; i++) fputc('-', out);
    fprintf(out, "+\n");

    /* header row */
    fprintf(out, "| %-*s | %-*s | %*s | %-*s |\n",
            cell_mode, "Modo", cell_route, "Ruta", cell_cost, "Costo total", cell_type, "Tipo");

    /* header sep */
    fprintf(out, "+");
    for (int i = 0; i < cell_mode + 2; i++) fputc('=', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_route + 2; i++) fputc('=', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_cost + 2; i++) fputc('=', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_type + 2; i++) fputc('=', out);
    fprintf(out, "+\n");

    for (int m = 0; m < MODE_COUNT; m++) {
        /* best row */
        char mode_text[64]; snprintf(mode_text, sizeof(mode_text), ">> %s", mode_name((TransportMode)m));
        char route_buf[1024] = ""; int pos = 0;
        if (results[m].has_best) {
            for (int i = 0; i < results[m].best.place_count; i++) {
                if (i > 0) pos += snprintf(route_buf + pos, sizeof(route_buf) - pos, " -> ");
                pos += snprintf(route_buf + pos, sizeof(route_buf) - pos, "%s", g->places[results[m].best.places[i]].name);
            }
        }
        if (route_buf[0] == '\0') snprintf(route_buf, sizeof(route_buf), "-");
        /* ensure displayed route fits into column: truncate with ellipses if necessary */
        char route_display[1024];
        size_t maxr = (size_t)cell_route;
        if (strlen(route_buf) > maxr) {
            if (maxr > 3) {
                strncpy(route_display, route_buf, maxr - 3);
                route_display[maxr - 3] = '\0';
                strcat(route_display, "...");
            } else {
                /* too narrow, hard truncate */
                strncpy(route_display, route_buf, maxr);
                route_display[maxr] = '\0';
            }
        } else {
            strcpy(route_display, route_buf);
        }
        char cost_str[64]; if (results[m].has_best && results[m].best.total_cost < INF_COST / 2.0) snprintf(cost_str, sizeof(cost_str), "%.2f", results[m].best.total_cost); else snprintf(cost_str, sizeof(cost_str), "-");
        /* print row with color-safe padded cells */
        fprintf(out, "| ");
        print_padded_colored(out, mode_text, cell_mode, mode_color((TransportMode)m), use_color, 0);
        fprintf(out, " | ");
        print_padded_colored(out, route_display, cell_route, NULL, 0, 0);
        fprintf(out, " | ");
        print_padded_colored(out, cost_str, cell_cost, NULL, 0, 1);
        fprintf(out, " | ");
        print_padded_colored(out, "Optima", cell_type, NULL, 0, 0);
        fprintf(out, " |\n");

        /* second row */
        snprintf(mode_text, sizeof(mode_text), "   %s", mode_name((TransportMode)m));
        route_buf[0] = '\0'; pos = 0;
        if (results[m].has_second) {
            for (int i = 0; i < results[m].second.place_count; i++) {
                if (i > 0) pos += snprintf(route_buf + pos, sizeof(route_buf) - pos, " -> ");
                pos += snprintf(route_buf + pos, sizeof(route_buf) - pos, "%s", g->places[results[m].second.places[i]].name);
            }
        }
        if (route_buf[0] == '\0') snprintf(route_buf, sizeof(route_buf), "-");
        /* prepare truncated display for second row */
        if (strlen(route_buf) > maxr) {
            if (maxr > 3) {
                strncpy(route_display, route_buf, maxr - 3);
                route_display[maxr - 3] = '\0';
                strcat(route_display, "...");
            } else {
                strncpy(route_display, route_buf, maxr);
                route_display[maxr] = '\0';
            }
        } else {
            strcpy(route_display, route_buf);
        }
        if (results[m].has_second && results[m].second.total_cost < INF_COST / 2.0) snprintf(cost_str, sizeof(cost_str), "%.2f", results[m].second.total_cost); else snprintf(cost_str, sizeof(cost_str), "-");
        fprintf(out, "| ");
        print_padded_colored(out, mode_text, cell_mode, mode_color((TransportMode)m), use_color, 0);
        fprintf(out, " | ");
        print_padded_colored(out, route_display, cell_route, NULL, 0, 0);
        fprintf(out, " | ");
        print_padded_colored(out, cost_str, cell_cost, NULL, 0, 1);
        fprintf(out, " | ");
        print_padded_colored(out, "Alternativa", cell_type, NULL, 0, 0);
        fprintf(out, " |\n");

        /* row separator */
        fprintf(out, "+");
        for (int i = 0; i < cell_mode + 2; i++) fputc('-', out);
        fprintf(out, "+");
        for (int i = 0; i < cell_route + 2; i++) fputc('-', out);
        fprintf(out, "+");
        for (int i = 0; i < cell_cost + 2; i++) fputc('-', out);
        fprintf(out, "+");
        for (int i = 0; i < cell_type + 2; i++) fputc('-', out);
        fprintf(out, "+\n");
    }
}

void output_results(const Graph *g, const PathResult *results)
{
    output_results_to_stream(stdout, g, results);
}
