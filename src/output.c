/**
 * @file output.c
 * @brief Implementación de la impresión formateada de resultados.
 *
 * Este archivo toma los resultados producidos por el algoritmo de Yen y los
 * presenta en una tabla de texto con columnas para modo de transporte, ruta,
 * costo total y tipo de ruta (óptima o alternativa). También administra el
 * uso opcional de colores ANSI cuando la salida es una terminal.
 */

#include "output.h"
#include "dijkstra.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

/** @brief Valor que representa un costo infinito. */
#define INF_COST 1e300

/**
 * @brief Devuelve el nombre legible de un modo de transporte.
 *
 * @param mode [in] Modo de transporte.
 * @return Cadena con el nombre del modo en español.
 */
static const char *mode_name(TransportMode mode)
{
    switch (mode) {
        case MODE_WALK: return "A pie";
        case MODE_BUS:  return "Bus";
        case MODE_CAR:  return "Carro";
        default:        return "?";
    }
}

/**
 * @brief Devuelve el código de color ANSI asociado a un modo de transporte.
 *
 * @param mode [in] Modo de transporte.
 * @return Cadena con el código ANSI correspondiente.
 */
static const char *mode_color(TransportMode mode)
{
    switch (mode) {
        case MODE_WALK: return "\x1b[32m"; /* verde */
        case MODE_BUS:  return "\x1b[34m"; /* azul */
        case MODE_CAR:  return "\x1b[35m"; /* magenta */
        default:        return "\x1b[0m";
    }
}

/**
 * @brief Calcula la longitud visible de una cadena de ruta.
 *
 * Suma las longitudes de los nombres de los lugares más los separadores
 * " -> " entre cada par de lugares. Si el camino está vacío, devuelve una
 * longitud de reserva.
 *
 * @param g    [in] Grafo con los nombres de los lugares.
 * @param path [in] Camino cuya longitud se desea calcular.
 * @return Longitud visible estimada de la cadena de ruta.
 */
static int route_string_length(const Graph *g, const Path *path)
{
    if (path->place_count == 0) return 22;
    int len = 0;
    for (int i = 0; i < path->place_count; i++) {
        len += (int)strlen(g->places[path->places[i]].name);
        // Separador entre nodos: " -> " (4 caracteres).
        if (i > 0) len += 4;
    }
    return len;
}

/**
 * @brief Calcula el ancho necesario para la columna de ruta.
 *
 * Revisa la longitud de las cadenas de ruta de las mejores y segundas rutas
 * de todos los modos, y devuelve un ancho mínimo razonable para la tabla.
 *
 * @param g       [in] Grafo con los nombres de los lugares.
 * @param results [in] Arreglo de resultados por modo de transporte.
 * @return Ancho de la columna de ruta.
 */
static int compute_route_width(const Graph *g, const PathResult *results)
{
    int width = 4; // "Ruta".
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

/**
 * @brief Imprime una cadena alineada y opcionalmente coloreada.
 *
 * Para mantener la alineación correcta cuando se usan códigos ANSI, primero
 * se prepara la cadena con relleno en un búfer local y luego se imprime el
 * búfer envuelto entre los códigos de color y reset.
 *
 * @param out         [in] Flujo de salida.
 * @param s           [in] Cadena a imprimir.
 * @param width       [in] Ancho de la columna.
 * @param color       [in] Código de color ANSI (puede ser NULL).
 * @param use_color   [in] Indica si se deben usar colores.
 * @param right_align [in] Indica si la cadena debe alinearse a la derecha.
 */
static void print_padded_colored(FILE *out, const char *s, int width, const char *color, int use_color, int right_align)
{
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

/**
 * @brief Muestra los resultados en un flujo de salida específico.
 *
 * Construye una tabla con los resultados de cada modo de transporte. Primero
 * calcula los anchos de columna adecuados, luego imprime el borde superior,
 * la fila de encabezados, el separador de encabezados y, para cada modo, dos
 * filas (óptima y alternativa). Gestiona la salida de colores según las
 * variables de entorno y si el flujo es una terminal.
 *
 * @param out     [in] Flujo donde se escriben los resultados.
 * @param g       [in] Grafo con los nombres de los lugares.
 * @param results [in] Arreglo de resultados por modo de transporte.
 */
void output_results_to_stream(FILE *out, const Graph *g, const PathResult *results)
{
    int route_width = compute_route_width(g, results);
    int use_color = 0;

#ifndef _WIN32
    // En sistemas Unix, detectamos si la salida es una terminal para decidir
    // si usar colores automáticamente.
    if (out == stdout) {
        use_color = isatty(STDOUT_FILENO);
    } else if (out == stderr) {
        use_color = isatty(STDERR_FILENO);
    } else {
        use_color = 0;
    }
#endif

    // Variables de entorno para forzar o deshabilitar colores.
    const char *env_no = getenv("MAPAS_NO_COLOR");
    if (!env_no) env_no = getenv("NO_COLOR");
    const char *env_yes = getenv("MAPAS_COLOR");
    if (env_no) use_color = 0;
    else if (env_yes) use_color = (*env_yes != '0');

    // Calcular los anchos de las columnas según el contenido real.
    int mode_w = 0;
    int cost_w = (int)strlen("Costo total");
    int type_w = (int)strlen("Alternativa");
    for (int m = 0; m < MODE_COUNT; m++) {
        int ml = (int)strlen(mode_name((TransportMode)m)) + 3; // prefijo >> y espacio.
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

    // Borde superior de la tabla.
    fprintf(out, "+");
    for (int i = 0; i < cell_mode + 2; i++) fputc('-', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_route + 2; i++) fputc('-', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_cost + 2; i++) fputc('-', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_type + 2; i++) fputc('-', out);
    fprintf(out, "+\n");

    // Fila de encabezados.
    fprintf(out, "| %-*s | %-*s | %*s | %-*s |\n",
            cell_mode, "Modo", cell_route, "Ruta", cell_cost, "Costo total", cell_type, "Tipo");

    // Separador de encabezados.
    fprintf(out, "+");
    for (int i = 0; i < cell_mode + 2; i++) fputc('=', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_route + 2; i++) fputc('=', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_cost + 2; i++) fputc('=', out);
    fprintf(out, "+");
    for (int i = 0; i < cell_type + 2; i++) fputc('=', out);
    fprintf(out, "+\n");

    // Imprimir una fila principal y una fila alternativa por cada modo.
    for (int m = 0; m < MODE_COUNT; m++) {
        // Fila de la mejor ruta.
        char mode_text[64]; snprintf(mode_text, sizeof(mode_text), ">> %s", mode_name((TransportMode)m));
        char route_buf[1024] = "";
        int pos = 0;
        if (results[m].has_best) {
            for (int i = 0; i < results[m].best.place_count; i++) {
                if (i > 0) pos += snprintf(route_buf + pos, sizeof(route_buf) - pos, " -> ");
                pos += snprintf(route_buf + pos, sizeof(route_buf) - pos, "%s", g->places[results[m].best.places[i]].name);
            }
        }
        if (route_buf[0] == '\0') snprintf(route_buf, sizeof(route_buf), "-");

        // Truncar la cadena si no cabe en la columna.
        char route_display[1024];
        size_t maxr = (size_t)cell_route;
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

        char cost_str[64];
        if (results[m].has_best && results[m].best.total_cost < INF_COST / 2.0)
            snprintf(cost_str, sizeof(cost_str), "%.2f", results[m].best.total_cost);
        else
            snprintf(cost_str, sizeof(cost_str), "-");

        // Imprimir la fila de la mejor ruta con color en el modo.
        fprintf(out, "| ");
        print_padded_colored(out, mode_text, cell_mode, mode_color((TransportMode)m), use_color, 0);
        fprintf(out, " | ");
        print_padded_colored(out, route_display, cell_route, NULL, 0, 0);
        fprintf(out, " | ");
        print_padded_colored(out, cost_str, cell_cost, NULL, 0, 1);
        fprintf(out, " | ");
        print_padded_colored(out, "Optima", cell_type, NULL, 0, 0);
        fprintf(out, " |\n");

        // Fila de la ruta alternativa.
        snprintf(mode_text, sizeof(mode_text), "   %s", mode_name((TransportMode)m));
        route_buf[0] = '\0'; pos = 0;
        if (results[m].has_second) {
            for (int i = 0; i < results[m].second.place_count; i++) {
                if (i > 0) pos += snprintf(route_buf + pos, sizeof(route_buf) - pos, " -> ");
                pos += snprintf(route_buf + pos, sizeof(route_buf) - pos, "%s", g->places[results[m].second.places[i]].name);
            }
        }
        if (route_buf[0] == '\0') snprintf(route_buf, sizeof(route_buf), "-");

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

        if (results[m].has_second && results[m].second.total_cost < INF_COST / 2.0)
            snprintf(cost_str, sizeof(cost_str), "%.2f", results[m].second.total_cost);
        else
            snprintf(cost_str, sizeof(cost_str), "-");

        fprintf(out, "| ");
        print_padded_colored(out, mode_text, cell_mode, mode_color((TransportMode)m), use_color, 0);
        fprintf(out, " | ");
        print_padded_colored(out, route_display, cell_route, NULL, 0, 0);
        fprintf(out, " | ");
        print_padded_colored(out, cost_str, cell_cost, NULL, 0, 1);
        fprintf(out, " | ");
        print_padded_colored(out, "Alternativa", cell_type, NULL, 0, 0);
        fprintf(out, " |\n");

        // Separador entre modos.
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

/**
 * @brief Muestra los resultados por la salida estándar.
 *
 * @param g       [in] Grafo con los nombres de los lugares.
 * @param results [in] Arreglo de resultados por modo de transporte.
 */
void output_results(const Graph *g, const PathResult *results)
{
    output_results_to_stream(stdout, g, results);
}
