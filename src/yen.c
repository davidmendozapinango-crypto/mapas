/**
 * @file yen.c
 * @brief Implementación del algoritmo de Yen para rutas alternativas.
 *
 * Este archivo calcula la mejor y la segunda mejor ruta entre dos lugares
 * para un modo de transporte dado. El algoritmo de Yen parte de la ruta
 * óptima obtenida con Dijkstra y explora rutas alternativas bloqueando
 * temporalmente nodos y aristas para forzar desviaciones.
 */

#include "yen.h"
#include "dijkstra.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/** @brief Valor que representa un costo infinito o un camino intransitable. */
#define INF_COST 1e300

/** @brief Tolerancia usada para comparar costos de punto flotante. */
#define EPS 1e-12

/**
 * @brief Compara dos caminos para determinar si representan la misma ruta.
 *
 * Dos caminos son iguales si tienen la misma cantidad de lugares y cada
 * posición contiene el mismo índice de lugar.
 *
 * @param a [in] Primer camino a comparar.
 * @param b [in] Segundo camino a comparar.
 * @return 1 si los caminos son iguales, 0 en caso contrario.
 */
static int paths_equal(const Path *a, const Path *b)
{
    if (a->place_count != b->place_count) return 0;
    for (int i = 0; i < a->place_count; i++) {
        if (a->places[i] != b->places[i]) return 0;
    }
    return 1;
}

/**
 * @brief Calcula el costo total de una secuencia de lugares.
 *
 * Dado un arreglo de índices de lugares, suma los costos efectivos de las
 * aristas consecutivas. Si entre dos lugares consecutivos no existe una
 * arista transitable, devuelve INF_COST.
 *
 * @param g     [in] Grafo que contiene las aristas.
 * @param mode  [in] Modo de transporte.
 * @param places [in] Arreglo de índices de lugares.
 * @param count [in] Cantidad de lugares en el arreglo.
 * @param wm    [in] Mapa climático (puede ser NULL).
 * @return Costo total de la secuencia, o INF_COST si no es transitable.
 */
static double path_cost(const Graph *g, TransportMode mode, const int *places,
                        int count, const WeatherMap *wm)
{
    double total = 0.0;
    for (int i = 0; i < count - 1; i++) {
        int from = places[i];
        int to = places[i + 1];
        double best = INF_COST;

        const EdgeList *list = &g->edges[from];
        for (int j = 0; j < list->count; j++) {
            const Route *r = &list->routes[j];
            if (r->to == to) {
                double c = weather_effective_cost(wm, r, mode);
                if (c < best) best = c;
            }
        }

        if (best >= INF_COST / 2.0) return INF_COST;
        total += best;
    }
    return total;
}

/**
 * @brief Agrega un camino candidato a la lista de candidatos.
 *
 * Si el candidato ya existe en la lista, no se agrega para evitar duplicados.
 * El arreglo de candidatos se redimensiona dinámicamente cuando es necesario.
 * Cada candidato almacena su propia copia del arreglo de lugares.
 *
 * @param candidates  [in,out] Puntero al arreglo de candidatos.
 * @param count       [in,out] Cantidad actual de candidatos.
 * @param capacity    [in,out] Capacidad reservada del arreglo.
 * @param candidate   [in]     Camino candidato a agregar.
 * @return 1 si se agregó correctamente, 0 si ya existía o hubo error.
 */
static int add_candidate(Path **candidates, int *count, int *capacity,
                         const Path *candidate)
{
    // Verificar si el candidato ya existe para evitar duplicados.
    for (int i = 0; i < *count; i++) {
        if (paths_equal(&(*candidates)[i], candidate)) {
            return 0;
        }
    }

    // Ampliar el arreglo si no hay espacio disponible.
    if (*count >= *capacity) {
        int new_cap = *capacity == 0 ? 4 : *capacity * 2;
        Path *new_items = realloc(*candidates, sizeof(Path) * (size_t)new_cap);
        if (!new_items) return 0;
        *candidates = new_items;
        *capacity = new_cap;
    }

    // Copiar el camino candidato, incluyendo su arreglo de lugares.
    Path copy;
    copy.place_count = candidate->place_count;
    copy.places = malloc(sizeof(int) * (size_t)candidate->place_count);
    if (!copy.places) return 0;
    memcpy(copy.places, candidate->places, sizeof(int) * (size_t)candidate->place_count);
    copy.total_cost = candidate->total_cost;
    copy.mode = candidate->mode;

    (*candidates)[*count] = copy;
    (*count)++;
    return 1;
}

/**
 * @brief Busca la mejor y segunda mejor ruta entre dos lugares.
 *
 * Algoritmo de Yen (variante para k=2):
 * 1. Obtiene la ruta óptima con Dijkstra.
 * 2. Para cada nodo intermedio de la ruta óptima, bloquea temporalmente los
 *    nodos anteriores y la arista que sigue inmediatamente después del nodo
 *    intermedio, y ejecuta Dijkstra para encontrar un camino alternativo.
 * 3. Combina el prefijo de la ruta óptima con el camino alternativo encontrado
 *    para formar candidatos.
 * 4. Selecciona el candidato con menor costo que sea distinto de la ruta óptima.
 *
 * @param g      [in]  Grafo con el mapa cargado.
 * @param mode   [in]  Modo de transporte a utilizar.
 * @param src    [in]  Índice del lugar de origen.
 * @param dst    [in]  Índice del lugar de destino.
 * @param wm     [in]  Mapa climático (puede ser NULL).
 * @param result [out] Estructura donde se guardan la mejor y segunda mejor ruta.
 * @return 1 si se encontró al menos la mejor ruta, 0 si no hay camino.
 */
int yen_find_paths(const Graph *g, TransportMode mode, int src, int dst,
                   const WeatherMap *wm, PathResult *result)
{
    // Inicializar el resultado en un estado vacío y seguro.
    result->best.places = NULL;
    result->best.place_count = 0;
    result->best.total_cost = 0.0;
    result->best.mode = mode;
    result->second = result->best;
    result->has_best = 0;
    result->has_second = 0;

    // Primero obtenemos la ruta óptima con Dijkstra sin bloqueos.
    Path best = dijkstra_find(g, mode, src, dst, wm, NULL, NULL, NULL, 0);
    if (best.place_count == 0) {
        return 0;
    }
    result->best = best;
    result->has_best = 1;

    // Lista de candidatos a segunda mejor ruta.
    Path *candidates = NULL;
    int candidate_count = 0;
    int candidate_capacity = 0;

    // Tomamos la ruta óptima como base para generar desviaciones.
    Path current = best;

    // k=2 significa que buscamos una segunda ruta alternativa. El bucle
    // externo se ejecuta una sola vez en este caso.
    int k = 2;
    for (int i = 0; i < k - 1; i++) {
        // Recorremos cada nodo intermedio de la ruta actual como punto de
        // desviación. El último nodo (destino) no puede ser punto de desvío.
        for (int spur_idx = 0; spur_idx < current.place_count - 1; spur_idx++) {
            int spur_node = current.places[spur_idx];

            // Bloqueamos los nodos que forman el prefijo de la ruta óptima
            // hasta el nodo de desviación. Esto fuerza a la alternativa a
            // divergir en el punto deseado.
            int *blocked_nodes = calloc((size_t)g->place_count, sizeof(int));
            if (!blocked_nodes) continue;
            for (int j = 0; j < spur_idx; j++) {
                blocked_nodes[current.places[j]] = 1;
            }

            // Preparamos arreglos para las aristas bloqueadas.
            int be_count = 0;
            int be_capacity = 4;
            int *be_from = malloc(sizeof(int) * (size_t)be_capacity);
            int *be_to = malloc(sizeof(int) * (size_t)be_capacity);
            if (!be_from || !be_to) {
                free(blocked_nodes);
                free(be_from);
                free(be_to);
                continue;
            }

            // En la versión actual solo consideramos la ruta óptima como
            // base, por lo que el conjunto de rutas previas contiene un
            // único elemento. Si se extendiera a k>2, aquí se iteraría sobre
            // todas las rutas ya encontradas.
            Path results[1] = { current };
            int result_count = 1;
            for (int p = 0; p < result_count; p++) {
                const Path *path = &results[p];
                if (spur_idx >= path->place_count - 1) continue;

                // Verificar si el prefijo de la ruta previa coincide con el
                // prefijo de la ruta actual hasta el punto de desviación.
                int same = 1;
                for (int j = 0; j <= spur_idx; j++) {
                    if (path->places[j] != current.places[j]) {
                        same = 0;
                        break;
                    }
                }

                // Si coincide, bloqueamos la arista que sigue al punto de
                // desviación para forzar una alternativa distinta.
                if (same) {
                    if (be_count >= be_capacity) {
                        be_capacity *= 2;
                        int *new_from = realloc(be_from, sizeof(int) * (size_t)be_capacity);
                        int *new_to = realloc(be_to, sizeof(int) * (size_t)be_capacity);
                        if (!new_from || !new_to) {
                            free(new_from);
                            free(new_to);
                            break;
                        }
                        be_from = new_from;
                        be_to = new_to;
                    }
                    be_from[be_count] = path->places[spur_idx];
                    be_to[be_count] = path->places[spur_idx + 1];
                    be_count++;
                }
            }

            // Ejecutamos Dijkstra desde el nodo de desviación hasta el
            // destino, usando los nodos y aristas bloqueados calculados.
            Path spur = dijkstra_find(g, mode, spur_node, dst, wm,
                                      blocked_nodes, be_from, be_to, be_count);
            if (spur.place_count > 0) {
                // Combinamos el prefijo de la ruta óptima con el camino
                // alternativo encontrado para formar un candidato completo.
                Path total;
                total.place_count = spur_idx + spur.place_count;
                total.places = malloc(sizeof(int) * (size_t)total.place_count);
                if (total.places) {
                    memcpy(total.places, current.places, sizeof(int) * (size_t)(spur_idx + 1));
                    memcpy(total.places + spur_idx + 1, spur.places + 1,
                           sizeof(int) * (size_t)(spur.place_count - 1));
                    total.mode = mode;

                    // Recalculamos el costo del prefijo más el costo del
                    // tramo alternativo, ya que el camino combinado puede
                    // incluir aristas que no están en la ruta original.
                    double root_cost = path_cost(g, mode, current.places, spur_idx + 1, wm);
                    total.total_cost = root_cost + spur.total_cost;

                    add_candidate(&candidates, &candidate_count, &candidate_capacity, &total);
                    free(total.places);
                }
            }

            // Liberar la memoria del camino alternativo y de los bloqueos.
            path_free(&spur);
            free(blocked_nodes);
            free(be_from);
            free(be_to);
        }
    }

    // Inicializamos la segunda mejor ruta con un costo infinito.
    Path second;
    second.places = NULL;
    second.place_count = 0;
    second.total_cost = INF_COST;
    second.mode = mode;

    // Recorremos los candidatos y nos quedamos con el de menor costo que sea
    // distinto a la ruta óptima. Liberamos los demás candidatos.
    for (int i = 0; i < candidate_count; i++) {
        if (!paths_equal(&best, &candidates[i]) &&
            candidates[i].total_cost < second.total_cost - EPS) {
            path_free(&second);
            second = candidates[i];
        } else {
            path_free(&candidates[i]);
        }
    }
    free(candidates);

    if (second.place_count > 0) {
        result->second = second;
        result->has_second = 1;
    }

    return 1;
}
