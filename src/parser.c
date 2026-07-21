/**
 * @file parser.c
 * @brief Implementación del cargador de archivos de rutas y clima.
 *
 * Este archivo contiene las funciones que leen los archivos de configuración
 * del programa. El archivo de rutas define los lugares y las conexiones
 * entre ellos. El archivo de clima define multiplicadores que ajustan los
 * costos de determinadas rutas.
 */

#include "parser.h"
#include "weather.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/** @brief Tamaño máximo de una línea leída de los archivos de configuración. */
#define LINE_SIZE 512

/**
 * @brief Indica si una línea está compuesta únicamente por espacios en blanco.
 *
 * @param line [in] Línea a evaluar.
 * @return 1 si la línea está vacía o solo tiene espacios, 0 en caso contrario.
 */
static int is_blank(const char *line) {
    while (*line) {
        if (!isspace((unsigned char)*line)) return 0;
        line++;
    }
    return 1;
}

/**
 * @brief Valida que un nombre solo contenga letras, dígitos o guiones bajos.
 *
 * Esta función se aplica a los nombres de lugares y de rutas leídos del
 * archivo, garantizando que no contengan espacios ni caracteres especiales.
 *
 * @param name [in] Cadena a validar.
 * @return 1 si el nombre es válido, 0 en caso contrario.
 */
static int is_valid_name(const char *name) {
    if (!name || name[0] == '\0') return 0;
    for (size_t i = 0; name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)name[i]) && name[i] != '_') return 0;
    }
    return 1;
}

/**
 * @brief Lee un archivo de rutas y carga su contenido en un grafo.
 *
 * El archivo debe tener la siguiente estructura:
 * - Primera línea: la palabra "Lugares".
 * - Líneas siguientes: nombres de lugares, uno por línea.
 * - Después de la lista de lugares: la palabra "Rutas".
 * - Líneas siguientes: rutas con el formato
 *   nombre_ruta -> origen:destino = P:costo; B:costo; C:costo.
 *
 * La función detecta y reporta errores de sintaxis, lugares duplicados,
 * costos negativos y rutas que referencian lugares desconocidos.
 *
 * @param filename  [in]  Ruta del archivo a leer.
 * @param g         [in,out] Grafo que se poblará.
 * @param error_msg [out] Búfer para el mensaje de error.
 * @param error_size [in] Tamaño del búfer de error.
 * @return 1 si el archivo se cargó correctamente, 0 en caso de error.
 */
int parse_route_file(const char *filename, Graph *g, char *error_msg, size_t error_size) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        snprintf(error_msg, error_size, "Error: no se pudo abrir el archivo de rutas \"%s\".", filename);
        return 0;
    }

    char line[LINE_SIZE];
    int line_no = 0;
    int in_places = 1;
    int in_routes = 0;
    int has_places = 0;
    int has_routes = 0;

    while (fgets(line, sizeof(line), f)) {
        line_no++;

        // Eliminar el carácter de nueva línea y el retorno de carro si
        // están presentes, para facilitar el análisis posterior.
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[--len] = '\0';
        if (len > 0 && line[len - 1] == '\r') line[--len] = '\0';
        if (is_blank(line)) continue;

        // La primera línea debe ser el encabezado "Lugares".
        if (line_no == 1) {
            if (strcmp(line, "Lugares") != 0) {
                snprintf(error_msg, error_size, "Error: sintaxis invalida en la linea %d del archivo de rutas. Se esperaba la cabecera 'Lugares'.", line_no);
                fclose(f);
                return 0;
            }
            continue;
        }

        // Dentro de la sección de lugares, se esperan nombres de lugares o la
        // cabecera "Rutas" que inicia la sección de rutas.
        if (in_places) {
            if (strcmp(line, "Rutas") == 0) {
                if (!has_places) {
                    snprintf(error_msg, error_size, "Error: sintaxis invalida en la linea %d del archivo de rutas. La seccion 'Rutas' debe aparecer despues de al menos un lugar.", line_no);
                    fclose(f);
                    return 0;
                }
                in_places = 0;
                in_routes = 1;
                continue;
            }
            if (strstr(line, "->") != NULL) {
                snprintf(error_msg, error_size, "Error: sintaxis invalida en la linea %d del archivo de rutas. Se esperaba la cabecera 'Rutas' antes de las rutas.", line_no);
                fclose(f);
                return 0;
            }
            if (!is_valid_name(line)) {
                snprintf(error_msg, error_size, "Error: sintaxis invalida en la linea %d del archivo de rutas. Nombre de lugar invalido: \"%s\".", line_no, line);
                fclose(f);
                return 0;
            }
            if (graph_find_place(g, line) >= 0) {
                fprintf(stderr, "Advertencia: nombre duplicado \"%s\" en la linea %d. Se usara la primera ocurrencia.\n", line, line_no);
                has_places = 1;
                continue;
            }
            int idx = graph_add_place(g, line);
            if (idx < 0) {
                snprintf(error_msg, error_size, "Error: no se pudo agregar el lugar en la linea %d.", line_no);
                fclose(f);
                return 0;
            }
            has_places = 1;
            continue;
        }

        // En la sección de rutas, cada línea debe ser una ruta válida.
        if (!in_routes) {
            snprintf(error_msg, error_size, "Error: sintaxis invalida en la linea %d del archivo de rutas. Se esperaba la cabecera 'Rutas' antes de las rutas.", line_no);
            fclose(f);
            return 0;
        }

        // Analizamos una línea de ruta con el formato esperado.
        char name[128], from[128], to[128];
        double p, b, c;
        if (sscanf(line, "%127s -> %127[^:]:%127s = P:%lf; B:%lf; C:%lf", name, from, to, &p, &b, &c) != 6) {
            snprintf(error_msg, error_size, "Error: sintaxis invalida en la linea %d del archivo de rutas. Formato esperado: nombre_ruta -> lugar_partida:lugar_llegada = P:costo; B:costo; C:costo", line_no);
            fclose(f);
            return 0;
        }
        if (!is_valid_name(name) || !is_valid_name(from) || !is_valid_name(to)) {
            snprintf(error_msg, error_size, "Error: sintaxis invalida en la linea %d del archivo de rutas. Identificadores invalidos.", line_no);
            fclose(f);
            return 0;
        }
        if (p < 0 || b < 0 || c < 0) {
            snprintf(error_msg, error_size, "Error: sintaxis invalida en la linea %d del archivo de rutas. Los costos no pueden ser negativos.", line_no);
            fclose(f);
            return 0;
        }
        if (graph_find_route(g, name)) {
            fprintf(stderr, "Advertencia: nombre duplicado \"%s\" en la linea %d. Se usara la primera ocurrencia.\n", name, line_no);
            has_routes = 1;
            continue;
        }
        if (!graph_add_route(g, name, from, to, p, b, c)) {
            snprintf(error_msg, error_size, "Error: la ruta en la linea %d referencia lugares desconocidos.", line_no);
            fclose(f);
            return 0;
        }
        has_routes = 1;
    }

    fclose(f);

    if (!has_places) {
        snprintf(error_msg, error_size, "Error: el archivo de rutas no contiene lugares.");
        return 0;
    }
    if (!has_routes) {
        snprintf(error_msg, error_size, "Error: el archivo de rutas no contiene rutas.");
        return 0;
    }

    return 1;
}

/**
 * @brief Lee un archivo de condiciones climáticas y lo carga en un mapa.
 *
 * Cada línea válida debe tener el formato:
 * nombre_ruta = P:multiplicador; B:multiplicador; C:multiplicador.
 *
 * Las líneas con formato incorrecto, coeficientes negativos o rutas que no
 * puedan almacenarse se ignoran con una advertencia, pero no detienen la
 * carga del resto del archivo.
 *
 * @param filename  [in]  Ruta del archivo a leer.
 * @param wm        [in,out] Mapa climático que se poblará.
 * @param error_msg [out] Búfer para el mensaje de error (reservado para uso futuro).
 * @param error_size [in] Tamaño del búfer de error.
 * @return 1 si el archivo se procesó correctamente.
 */
int parse_weather_file(const char *filename, WeatherMap *wm, char *error_msg, size_t error_size) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        snprintf(error_msg, error_size, "Error: no se pudo abrir el archivo de clima \"%s\".", filename);
        return 0;
    }

    char line[LINE_SIZE];
    int line_no = 0;

    while (fgets(line, sizeof(line), f)) {
        line_no++;

        // Normalizar la línea eliminando saltos y retornos de carro.
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[--len] = '\0';
        if (len > 0 && line[len - 1] == '\r') line[--len] = '\0';
        if (is_blank(line)) continue;

        char name[128];
        double p, b, c;
        if (sscanf(line, "%127s = P:%lf; B:%lf; C:%lf", name, &p, &b, &c) != 4) {
            fprintf(stderr, "Advertencia: coeficiente invalido en la linea %d del archivo de clima. Se esperaba un valor numerico para P, B o C. Linea ignorada.\n", line_no);
            continue;
        }
        if (p < 0 || b < 0 || c < 0) {
            fprintf(stderr, "Advertencia: coeficiente invalido en la linea %d del archivo de clima. Los coeficientes no pueden ser negativos. Linea ignorada.\n", line_no);
            continue;
        }
        if (!weather_add(wm, name, p, b, c)) {
            fprintf(stderr, "Advertencia: no se pudo guardar la entrada de clima en la linea %d. Linea ignorada.\n", line_no);
        }
    }

    fclose(f);
    (void)error_size;
    return 1;
}
