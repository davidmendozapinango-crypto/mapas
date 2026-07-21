/**
 * @file cli.c
 * @brief Implementación de la interfaz de línea de comandos del programa.
 *
 * Este archivo coordina la interacción con el usuario: solicita los archivos
 * de rutas y clima, valida los lugares de origen y destino, invoca los
 * algoritmos de búsqueda de rutas y muestra los resultados. También permite
 * ejecutar la interfaz con flujos de entrada y salida personalizados para
 * facilitar las pruebas automatizadas.
 */

#include "cli.h"
#include "parser.h"
#include "dijkstra.h"
#include "yen.h"
#include "output.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/** @brief Tamaño máximo de los búferes usados para leer líneas del usuario. */
#define LINE_SIZE 512

/**
 * @brief Muestra un prompt y lee una línea desde un flujo de entrada.
 *
 * Si se proporciona un prompt, lo escribe en el flujo de salida y fuerza la
 * salida inmediata con fflush. Luego lee una línea completa con fgets y
 * elimina los caracteres de nueva línea y retorno de carro al final.
 *
 * @param prompt [in]  Texto del prompt a mostrar (puede ser NULL).
 * @param in     [in]  Flujo desde el que se lee.
 * @param out    [in]  Flujo donde se escribe el prompt.
 * @param buf    [out] Búfer donde se almacena la línea leída.
 * @param size   [in]  Tamaño máximo del búfer, incluyendo el carácter nulo.
 * @return 1 si se leyó una línea, 0 si se alcanzó el fin de archivo o hubo error.
 */
int cli_prompt_line(const char *prompt, FILE *in, FILE *out, char *buf, size_t size)
{
    if (prompt) {
        fprintf(out, "%s", prompt);
        fflush(out);
    }

    if (!fgets(buf, (int)size, in)) {
        return 0;
    }

    // Eliminar el salto de línea y el retorno de carro si están presentes.
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[--len] = '\0';
    if (len > 0 && buf[len - 1] == '\r') buf[--len] = '\0';

    return 1;
}

/**
 * @brief Carga un archivo de rutas en un grafo.
 *
 * Crea un grafo nuevo, intenta cargar el archivo y, si la carga es exitosa,
 * reemplaza el grafo anterior. Si ocurre un error, libera el grafo parcial y
 * escribe el mensaje en el búfer de error.
 *
 * @param g        [in,out] Puntero al puntero del grafo a cargar.
 * @param filename [in]     Ruta del archivo de rutas.
 * @param error    [out]    Búfer donde escribir el mensaje de error.
 * @param err_size [in]     Tamaño del búfer de error.
 * @return 1 si el archivo se cargó correctamente, 0 en caso de error.
 */
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

    // Si ya había un grafo cargado, lo liberamos antes de reemplazarlo.
    if (*g) graph_destroy(*g);
    *g = newg;
    return 1;
}

/**
 * @brief Carga un archivo de condiciones climáticas.
 *
 * Si el filename es una cadena vacía, crea un mapa climático vacío para que
 * el programa use los costos base. Si el filename contiene una ruta, intenta
 * cargar el archivo correspondiente. En cualquier caso, reemplaza el mapa
 * climático anterior.
 *
 * @param wm       [in,out] Puntero al puntero del mapa climático a cargar.
 * @param filename [in]     Ruta del archivo de clima, o cadena vacía para omitir.
 * @param error    [out]    Búfer donde escribir el mensaje de error.
 * @param err_size [in]     Tamaño del búfer de error.
 * @return 1 si se cargó correctamente o se omitió de forma válida, 0 en error.
 */
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

    // Si el usuario no proporcionó archivo, dejamos el mapa vacío.
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

/**
 * @brief Valida que el origen y el destino sean lugares distintos del grafo.
 *
 * Verifica que ambos nombres existan en el grafo y que no sean iguales. Si
 * alguna validación falla, escribe un mensaje descriptivo en el búfer de error.
 *
 * @param g      [in]  Grafo con los lugares cargados.
 * @param origin [in]  Nombre del lugar de origen.
 * @param dest   [in]  Nombre del lugar de destino.
 * @param error  [out] Búfer donde escribir el mensaje de error.
 * @param err_size [in] Tamaño del búfer de error.
 * @return 1 si origen y destino son válidos y distintos, 0 en caso contrario.
 */
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

/**
 * @brief Calcula y muestra las rutas óptimas y alternativas para un par origen-destino.
 *
 * Para cada modo de transporte, invoca yen_find_paths para obtener la mejor y
 * la segunda mejor ruta, y luego imprime los resultados formateados en el
 * flujo de salida indicado. Finalmente libera la memoria interna de los
 * caminos obtenidos.
 *
 * @param out    [in]  Flujo donde se escriben los resultados.
 * @param g      [in]  Grafo con el mapa cargado.
 * @param wm     [in]  Mapa climático (puede ser NULL).
 * @param origin [in]  Nombre del lugar de origen.
 * @param dest   [in]  Nombre del lugar de destino.
 */
void cli_compute_and_output(FILE *out, const Graph *g, const WeatherMap *wm,
                            const char *origin, const char *dest)
{
    int src = graph_find_place(g, origin);
    int dst = graph_find_place(g, dest);

    // Arreglo con un resultado por cada modo de transporte.
    PathResult results[MODE_COUNT];
    for (int m = 0; m < MODE_COUNT; m++) {
        results[m].has_best = 0;
        results[m].has_second = 0;
    }

    // Calcular rutas para cada modo de transporte.
    for (int m = 0; m < MODE_COUNT; m++) {
        yen_find_paths(g, (TransportMode)m, src, dst, wm, &results[m]);
    }

    output_results_to_stream(out, g, results);

    // Liberar la memoria de cada camino obtenido.
    for (int m = 0; m < MODE_COUNT; m++) {
        path_free(&results[m].best);
        path_free(&results[m].second);
    }
}

/**
 * @brief Interpreta la primera letra de una acción del usuario.
 *
 * Convierte la primera letra de la entrada a minúscula y devuelve un código
 * numérico según la acción elegida.
 *
 * @param input [in] Cadena introducida por el usuario.
 * @return 0 para nueva ruta, 1 para cambiar archivos, 2 para salir, -1 si es inválida.
 */
int cli_parse_action(const char *input)
{
    if (input == NULL || input[0] == '\0') return -1;
    char c = (char)tolower((unsigned char)input[0]);
    if (c == 'n') return 0;
    if (c == 'c') return 1;
    if (c == 's') return 2;
    return -1;
}

/**
 * @brief Ejecuta la interfaz de línea de comandos usando flujos personalizados.
 *
 * Controla el flujo principal de la aplicación: carga de archivos, consultas
 * de rutas y menú de acciones. Permite usar flujos de entrada y salida
 * arbitrarios para facilitar las pruebas.
 *
 * @param in  [in]  Flujo de entrada.
 * @param out [in]  Flujo de salida.
 * @return 0 cuando la ejecución termina de forma normal.
 */
int cli_run_with_streams(FILE *in, FILE *out)
{
    Graph *g = NULL;
    WeatherMap *wm = NULL;
    char buf[LINE_SIZE];
    int running = 1;

    fprintf(out, "Bienvenido a Mapas.\n");

    while (running) {
        int loaded = 0;

        // Bucle para cargar el archivo de rutas. No se continúa hasta que el
        // archivo se cargue correctamente o el usuario cierre la entrada.
        while (!loaded) {
            if (!cli_prompt_line("Ruta del archivo de rutas: ", in, out, buf, sizeof(buf))) {
                running = 0;
                break;
            }

            char error[LINE_SIZE];
            if (cli_load_route_file(&g, buf, error, sizeof(error))) {
                loaded = 1;
                fprintf(out, "Archivo de rutas cargado correctamente: %s\n", buf);
                // Mostrar la lista de lugares disponibles para ayudar al usuario.
                fprintf(out, "Lugares disponibles:\n");
                for (int i = 0; i < g->place_count; i++) {
                    fprintf(out, " - %s\n", g->places[i].name);
                }
                fflush(out);
            } else {
                fprintf(out, "%s\n", error);
            }
        }
        if (!running) break;

        // Bucle para cargar el archivo de clima. Permite omitirlo.
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
                if (buf[0] == '\0') {
                    fprintf(out, "Sin archivo de clima. Se usaran los costes base.\n");
                } else {
                    fprintf(out, "Archivo de clima cargado correctamente: %s\n", buf);
                }
                fflush(out);
            } else {
                fprintf(out, "%s\n", error);
            }
        }
        if (!running) break;

        // Advertir sobre rutas climáticas que no existen en el grafo.
        weather_report_unknown_routes(wm, g);

        // Bucle de consultas de rutas. Permite hacer múltiples consultas con
        // los mismos archivos cargados hasta que el usuario elija cambiar
        // archivos o salir.
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

            // Menú de acciones: nueva ruta, cambiar archivos o salir.
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

    // Liberar la memoria del grafo y del mapa climático antes de terminar.
    if (g) graph_destroy(g);
    if (wm) weather_destroy(wm);
    return 0;
}

/**
 * @brief Ejecuta la interfaz de línea de comandos con stdin y stdout.
 *
 * Es la función que se invoca desde main para iniciar la aplicación de forma
 * interactiva en la consola.
 *
 * @return 0 cuando la ejecución termina de forma normal.
 */
int cli_run(void)
{
    return cli_run_with_streams(stdin, stdout);
}
