/**
 * @file output.h
 * @brief Interfaz para la impresión formateada de resultados.
 *
 * Este módulo toma los resultados producidos por el algoritmo de Yen y los
 * presenta en forma de tabla legible, incluyendo la mejor ruta y la ruta
 * alternativa para cada modo de transporte.
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include "types.h"
#include "graph.h"
#include <stdio.h>

/**
 * @brief Muestra los resultados por la salida estándar.
 *
 * Es un atajo conveniente que llama a output_results_to_stream con stdout.
 *
 * @param g       [in] Grafo con los nombres de los lugares.
 * @param results [in] Arreglo de resultados por modo de transporte.
 */
void output_results(const Graph *g, const PathResult *results);

/**
 * @brief Muestra los resultados en un flujo de salida específico.
 *
 * Imprime una tabla con columnas para modo de transporte, ruta, costo total
 * y tipo (óptima o alternativa). Adapta los anchos de columna al contenido y
 * permite usar colores ANSI cuando la salida es una terminal.
 *
 * @param out     [in] Flujo donde se escriben los resultados.
 * @param g       [in] Grafo con los nombres de los lugares.
 * @param results [in] Arreglo de resultados por modo de transporte.
 */
void output_results_to_stream(FILE *out, const Graph *g, const PathResult *results);

#endif
