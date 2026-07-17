#include "weather.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INF_COST 1e300

WeatherMap *weather_create(int initial_capacity) {
    WeatherMap *wm = malloc(sizeof(WeatherMap));
    if (!wm) return NULL;
    wm->count = 0;
    wm->capacity = initial_capacity > 0 ? initial_capacity : 16;
    wm->conditions = malloc(sizeof(WeatherCondition) * wm->capacity);
    if (!wm->conditions) {
        free(wm);
        return NULL;
    }
    return wm;
}

void weather_destroy(WeatherMap *wm) {
    if (!wm) return;
    for (int i = 0; i < wm->count; i++) {
        free(wm->conditions[i].route_name);
    }
    free(wm->conditions);
    free(wm);
}

static int find_index(const WeatherMap *wm, const char *route_name) {
    for (int i = 0; i < wm->count; i++) {
        if (strcmp(wm->conditions[i].route_name, route_name) == 0) return i;
    }
    return -1;
}

int weather_add(WeatherMap *wm, const char *route_name, double walk, double bus, double car) {
    if (!wm || !route_name) return 0;
    int idx = find_index(wm, route_name);
    if (idx < 0) {
        if (wm->count >= wm->capacity) {
            int new_cap = wm->capacity * 2;
            WeatherCondition *new_conds = realloc(wm->conditions, sizeof(WeatherCondition) * new_cap);
            if (!new_conds) return 0;
            wm->conditions = new_conds;
            wm->capacity = new_cap;
        }
        idx = wm->count++;
        wm->conditions[idx].route_name = xstrdup(route_name);
        if (!wm->conditions[idx].route_name) return 0;
    }
    wm->conditions[idx].multipliers[MODE_WALK] = walk;
    wm->conditions[idx].multipliers[MODE_BUS] = bus;
    wm->conditions[idx].multipliers[MODE_CAR] = car;
    return 1;
}

double weather_effective_cost(const WeatherMap *wm, const Route *route, TransportMode mode) {
    if (!route) return INF_COST;
    double base = route->costs[mode];
    if (base <= 0.0) return INF_COST;
    double mult = 1.0;
    if (wm) {
        int idx = find_index(wm, route->name);
        if (idx >= 0) {
            mult = wm->conditions[idx].multipliers[mode];
            if (mult <= 0.0) return INF_COST;
        }
    }
    return base * mult;
}

int weather_has_route(const WeatherMap *wm, const char *route_name) {
    if (!wm || !route_name) return 0;
    return find_index(wm, route_name) >= 0;
}

void weather_report_unknown_routes(const WeatherMap *wm, const Graph *g) {
    if (!wm || !g) return;
    for (int i = 0; i < wm->count; i++) {
        if (!graph_find_route(g, wm->conditions[i].route_name)) {
            fprintf(stderr, "Advertencia: la linea %d del clima referencia la ruta desconocida \"%s\" y sera ignorada.\n",
                    i + 1, wm->conditions[i].route_name);
        }
    }
}
