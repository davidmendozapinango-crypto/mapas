#include "cli.h"
#include "parser.h"
#include "dijkstra.h"
#include "yen.h"
#include "output.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_SIZE 512

int cli_prompt_line(const char *prompt, FILE *in, FILE *out, char *buf, size_t size)
{
    if (prompt) {
        fprintf(out, "%s", prompt);
        fflush(out);
    }
    if (!fgets(buf, (int)size, in)) {
        return 0;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[--len] = '\0';
    if (len > 0 && buf[len - 1] == '\r') buf[--len] = '\0';
    return 1;
}

int cli_load_route_file(Graph **g, const char *filename, char *error, size_t err_size)
{
    Graph *newg = graph_create(16);
    if (!newg) {
        snprintf(error, err_size, "Error: no se pudo reservar memoria para el mapa.");
        return 0;
    }
    if (!parse_route_file(filename, newg, error, err_size)) {
        graph_destroy(newg);
        return 0;
    }
    if (*g) graph_destroy(*g);
    *g = newg;
    return 1;
}

int cli_load_weather_file(WeatherMap **wm, const char *filename, char *error, size_t err_size)
{
    if (*wm) {
        weather_destroy(*wm);
    }
    WeatherMap *new_wm = weather_create(16);
    if (!new_wm) {
        snprintf(error, err_size, "Error: no se pudo reservar memoria para el clima.");
        return 0;
    }
    if (filename[0] == '\0') {
        *wm = new_wm;
        return 1;
    }
    if (!parse_weather_file(filename, new_wm, error, err_size)) {
        weather_destroy(new_wm);
        return 0;
    }
    *wm = new_wm;
    return 1;
}

int cli_validate_origin_destination(const Graph *g, const char *origin, const char *dest,
                                    char *error, size_t err_size)
{
    if (graph_find_place(g, origin) < 0) {
        snprintf(error, err_size, "Error: \"%s\" no es un lugar conocido en el mapa cargado.", origin);
        return 0;
    }
    if (graph_find_place(g, dest) < 0) {
        snprintf(error, err_size, "Error: \"%s\" no es un lugar conocido en el mapa cargado.", dest);
        return 0;
    }
    if (strcmp(origin, dest) == 0) {
        snprintf(error, err_size, "Error: el destino debe ser diferente del origen.");
        return 0;
    }
    return 1;
}

void cli_compute_and_output(FILE *out, const Graph *g, const WeatherMap *wm,
                            const char *origin, const char *dest)
{
    int src = graph_find_place(g, origin);
    int dst = graph_find_place(g, dest);
    PathResult results[MODE_COUNT];
    for (int m = 0; m < MODE_COUNT; m++) {
        results[m].has_best = 0;
        results[m].has_second = 0;
    }

    for (int m = 0; m < MODE_COUNT; m++) {
        yen_find_paths(g, (TransportMode)m, src, dst, wm, &results[m]);
    }

    output_results_to_stream(out, g, results);

    for (int m = 0; m < MODE_COUNT; m++) {
        path_free(&results[m].best);
        path_free(&results[m].second);
    }
}

int cli_parse_action(const char *input)
{
    if (input == NULL || input[0] == '\0') return -1;
    char c = (char)tolower((unsigned char)input[0]);
    if (c == 'n') return 0;
    if (c == 'c') return 1;
    if (c == 's') return 2;
    return -1;
}

int cli_run_with_streams(FILE *in, FILE *out)
{
    Graph *g = NULL;
    WeatherMap *wm = NULL;
    char buf[LINE_SIZE];
    int running = 1;

    fprintf(out, "Bienvenido a Mapas.\n");

    while (running) {
        int loaded = 0;
        while (!loaded) {
            if (!cli_prompt_line("Ruta del archivo de rutas: ", in, out, buf, sizeof(buf))) {
                running = 0;
                break;
            }
            char error[LINE_SIZE];
            if (cli_load_route_file(&g, buf, error, sizeof(error))) {
                loaded = 1;
            } else {
                fprintf(out, "%s\n", error);
            }
        }
        if (!running) break;

        loaded = 0;
        while (!loaded) {
            if (!cli_prompt_line("Ruta del archivo de clima (opcional, presione Enter para omitir): ",
                                 in, out, buf, sizeof(buf))) {
                running = 0;
                break;
            }
            char error[LINE_SIZE];
            if (cli_load_weather_file(&wm, buf, error, sizeof(error))) {
                loaded = 1;
            } else {
                fprintf(out, "%s\n", error);
            }
        }
        if (!running) break;

        weather_report_unknown_routes(wm, g);

        int query = 1;
        while (query) {
            int valid = 0;
            char origin[LINE_SIZE];
            while (!valid) {
                if (!cli_prompt_line("Lugar de origen: ", in, out, origin, sizeof(origin))) {
                    running = 0;
                    break;
                }
                char dest[LINE_SIZE];
                if (!cli_prompt_line("Lugar de destino: ", in, out, dest, sizeof(dest))) {
                    running = 0;
                    break;
                }
                char error[LINE_SIZE];
                if (cli_validate_origin_destination(g, origin, dest, error, sizeof(error))) {
                    cli_compute_and_output(out, g, wm, origin, dest);
                    valid = 1;
                } else {
                    fprintf(out, "%s\n", error);
                }
            }
            if (!running) break;

            int action = -1;
            while (action < 0) {
                if (!cli_prompt_line("Elija una accion: [n]ueva ruta, [c]ambiar archivos, [s]alir ",
                                     in, out, buf, sizeof(buf))) {
                    running = 0;
                    break;
                }
                action = cli_parse_action(buf);
                if (action < 0) {
                    fprintf(out, "Error: opcion invalida. Ingrese n, c o s.\n");
                }
            }
            if (!running) break;

            if (action == 0) {
                query = 1;
            } else if (action == 1) {
                query = 0;
            } else {
                running = 0;
                query = 0;
            }
        }
    }

    if (g) graph_destroy(g);
    if (wm) weather_destroy(wm);
    return 0;
}

int cli_run(void)
{
    return cli_run_with_streams(stdin, stdout);
}
