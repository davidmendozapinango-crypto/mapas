/**
 * @file dijkstra.h
 * @brief Interfaz del algoritmo de Dijkstra para cálculo de rutas mínimas.
 *
 * Este módulo proporciona una función para encontrar el camino de menor costo
 * entre dos lugares de un grafo, considerando un modo de transporte, un mapa
 * climático opcional y listas opcionales de nodos y aristas bloqueadas. El
 * algoritmo de Dijkstra garantiza la ruta óptima cuando todos los pesos son
 * no negativos.
 */

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "types.h"
#include "graph.h"
#include "weather.h"

/**
 * @brief Calcula el camino de costo mínimo entre dos lugares.
 *
 * Aplica el algoritmo de Dijkstra usando un min-heap como cola de prioridad.
 * El costo de cada arista se obtiene a través de weather_effective_cost, que
 * puede modificar el costo base según las condiciones climáticas. Las listas
 * de nodos y aristas bloqueadas permiten al algoritmo de Yen explorar rutas
 * alternativas.
 *
 * @param g                  [in] Grafo sobre el que se busca.
 * @param mode               [in] Modo de transporte a utilizar.
 * @param src                [in] Índice del lugar de origen.
 * @param dst                [in] Índice del lugar de destino.
 * @param wm                 [in] Mapa climático (puede ser NULL).
 * @param blocked_nodes      [in] Arreglo de nodos bloqueados (puede ser NULL).
 * @param blocked_edges_from [in] Orígenes de aristas bloqueadas (puede ser NULL).
 * @param blocked_edges_to   [in] Destinos de aristas bloqueadas (puede ser NULL).
 * @param blocked_edge_count [in] Cantidad de aristas bloqueadas.
 * @return Estructura Path con la ruta encontrada. Si no hay ruta, place_count
 *         será 0 y total_cost será infinito.
 */
Path dijkstra_find(const Graph *g, TransportMode mode, int src, int dst,
                   const WeatherMap *wm, const int *blocked_nodes,
                   const int *blocked_edges_from, const int *blocked_edges_to,
                   int blocked_edge_count);

/**
 * @brief Libera la memoria interna de una estructura Path.
 *
 * Libera el arreglo de lugares y restablece los campos a valores seguros.
 * Si el puntero es NULL, la función no hace nada.
 *
 * @param p [in,out] Camino cuya memoria se desea liberar.
 */
void path_free(Path *p);

#endif
