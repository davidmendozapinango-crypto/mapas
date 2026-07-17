#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include "graph.h"
#include "weather.h"

int cli_run(void);
int cli_run_with_streams(FILE *in, FILE *out);
int cli_prompt_line(const char *prompt, FILE *in, FILE *out, char *buf, size_t size);
int cli_load_route_file(Graph **g, const char *filename, char *error, size_t err_size);
int cli_load_weather_file(WeatherMap **wm, const char *filename, char *error, size_t err_size);
int cli_validate_origin_destination(const Graph *g, const char *origin, const char *dest,
                                    char *error, size_t err_size);
void cli_compute_and_output(FILE *out, const Graph *g, const WeatherMap *wm,
                            const char *origin, const char *dest);
int cli_parse_action(const char *input);

#endif
