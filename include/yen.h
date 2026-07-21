/**
 * @file yen.h
 * @brief Interfaz del algoritmo de Yen para encontrar rutas alternativas.
 *
 * Este módulo calcula la mejor y la segunda mejor ruta entre dos lugares para
 * un modo de transporte dado. Se apoya en el algoritmo de Dijkstra para
 * encontrar rutas candidatas mientras bloquea temporalmente nodos y aristas.
 */

#ifndef YEN_H
#define YEN_H

#include "types.h"
#include "graph.h"
#include "weather.h"

/**
 * @brief Busca la mejor y segunda mejor ruta entre dos lugares.
 *
 * Utiliza el algoritmo de Yen para obtener, a partir de la ruta óptima de
 * Dijkstra, una segunda opción distinta. El resultado se escribe en la
 * estructura PathResult apuntada por result.
 *
 * @param g     [in]  Grafo con el mapa cargado.
 * @param mode  [in]  Modo de transporte a utilizar.
 * @param src   [in]  Índice del lugar de origen.
 * @param dst   [in]  Índice del lugar de destino.
 * @param wm    [in]  Mapa climático (puede ser NULL).
 * @param result [out] Estructura donde se guardan las rutas encontradas.
 * @return 1 si se encontró al menos la mejor ruta, 0 si no hay camino.
 */
int yen_find_paths(const Graph *g, TransportMode mode, int src, int dst,
                   const WeatherMap *wm, PathResult *result);

#endif
