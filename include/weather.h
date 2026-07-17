#ifndef WEATHER_H
#define WEATHER_H

#include "types.h"
#include "graph.h"

typedef struct {
    char *route_name;
    double multipliers[MODE_COUNT];
} WeatherCondition;

typedef struct {
    WeatherCondition *conditions;
    int count;
    int capacity;
} WeatherMap;

WeatherMap *weather_create(int initial_capacity);
void weather_destroy(WeatherMap *wm);
int weather_add(WeatherMap *wm, const char *route_name, double walk, double bus, double car);
double weather_effective_cost(const WeatherMap *wm, const Route *route, TransportMode mode);
int weather_has_route(const WeatherMap *wm, const char *route_name);
void weather_report_unknown_routes(const WeatherMap *wm, const Graph *g);

#endif
