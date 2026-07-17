#ifndef PARSER_H
#define PARSER_H

#include "graph.h"
#include "weather.h"

int parse_route_file(const char *filename, Graph *g, char *error_msg, size_t error_size);
int parse_weather_file(const char *filename, WeatherMap *wm, char *error_msg, size_t error_size);

#endif
