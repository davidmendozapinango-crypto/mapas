/**
 * @file graph.h
 * @brief Interfaz pública para la gestión de grafos de rutas.
 *
 * Este módulo encapsula la creación, destrucción y manipulación del grafo
 * dirigido que representa el mapa de la ciudad. Cada lugar es un vértice y
 * cada ruta es una arista dirigida que conecta dos lugares. Los costos se
 * almacenan por modo de transporte para permitir consultas posteriores.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "types.h"

/**
 * @brief Lista de rutas que salen desde un lugar específico.
 *
 * El grafo utiliza un arreglo de estas listas: la posición i del arreglo
 * corresponde al lugar con índice i, y almacena todas las rutas que parten
 * de él. Esta representación es eficiente para recorrer los vecinos de un
 * nodo durante los algoritmos de búsqueda.
 */
typedef struct {
    Route *routes; /**< Arreglo dinámico de rutas salientes. */
    int count;     /**< Cantidad de rutas válidas almacenadas. */
    int capacity;  /**< Capacidad reservada del arreglo routes. */
} EdgeList;

/**
 * @brief Grafo dirigido que representa el mapa de lugares y rutas.
 *
 * El grafo mantiene dos arreglos paralelos: uno de lugares y otro de listas
 * de aristas. La capacidad de ambos arreglos crece dinámicamente conforme se
 * agregan más lugares.
 */
typedef struct {
    Place *places;     /**< Arreglo dinámico de lugares (vértices). */
    int place_count;   /**< Cantidad de lugares almacenados. */
    int place_capacity; /**< Capacidad reservada del arreglo places. */
    EdgeList *edges;   /**< Arreglo dinámico de listas de aristas. */
} Graph;

/**
 * @brief Crea un grafo vacío con una capacidad inicial.
 *
 * Reserva memoria para los arreglos internos de lugares y aristas. Si la
 * capacidad solicitada es menor o igual a cero, se usa un valor por defecto.
 *
 * @param initial_capacity Capacidad inicial deseada para el arreglo de lugares.
 * @return Puntero al grafo creado, o NULL si falla la reserva de memoria.
 */
Graph *graph_create(int initial_capacity);

/**
 * @brief Libera toda la memoria asociada a un grafo.
 *
 * Recorre cada lugar liberando sus nombres, luego libera cada lista de rutas
 * y sus nombres, y finalmente libera los arreglos y la estructura principal.
 * Si el puntero es NULL, la función no hace nada.
 *
 * @param g [in] Grafo a destruir.
 */
void graph_destroy(Graph *g);

/**
 * @brief Agrega un nuevo lugar al grafo.
 *
 * Si el lugar ya existe, devuelve su índice actual sin duplicarlo. El nombre
 * se copia en memoria dinámica, por lo que el llamador puede liberar o
 * reutilizar la cadena original después de la llamada.
 *
 * @param g    [in,out] Grafo al que se agrega el lugar.
 * @param name [in]     Nombre del lugar a agregar.
 * @return Índice del lugar dentro del grafo, o -1 si ocurre un error.
 */
int graph_add_place(Graph *g, const char *name);

/**
 * @brief Busca el índice de un lugar por su nombre.
 *
 * Realiza una búsqueda lineal sobre el arreglo de lugares comparando cadenas.
 *
 * @param g    [in] Grafo en el que se busca.
 * @param name [in] Nombre del lugar a buscar.
 * @return Índice del lugar, o -1 si no se encuentra o los parámetros son inválidos.
 */
int graph_find_place(const Graph *g, const char *name);

/**
 * @brief Agrega una ruta dirigida entre dos lugares del grafo.
 *
 * Los lugares de origen y destino deben existir previamente en el grafo. La
 * ruta se almacena en la lista de aristas del lugar de origen. Los costos
 * menores o iguales a cero indican que el modo correspondiente no está
 * disponible.
 *
 * @param g          [in,out] Grafo al que se agrega la ruta.
 * @param name       [in]     Nombre identificador de la ruta.
 * @param from       [in]     Nombre del lugar de origen.
 * @param to         [in]     Nombre del lugar de destino.
 * @param walk_cost  [in]     Costo para ir a pie.
 * @param bus_cost   [in]     Costo para ir en bus.
 * @param car_cost   [in]     Costo para ir en carro.
 * @return 1 si la ruta se agregó correctamente, 0 en caso de error.
 */
int graph_add_route(Graph *g, const char *name, const char *from, const char *to,
                    double walk_cost, double bus_cost, double car_cost);

/**
 * @brief Obtiene una ruta específica desde la lista de aristas de un lugar.
 *
 * Esta función permite acceder a una arista por su posición dentro de la lista
 * de rutas salientes de un lugar. Es útil para iterar sobre todas las rutas
 * de un nodo sin acceder directamente a los campos internos del grafo.
 *
 * @param g    [in] Grafo que contiene la ruta.
 * @param from [in] Índice del lugar de origen.
 * @param idx  [in] Posición de la ruta dentro de la lista de aristas.
 * @return Puntero a la ruta solicitada, o NULL si los índices son inválidos.
 */
const Route *graph_get_edge(const Graph *g, int from, int idx);

/**
 * @brief Devuelve la cantidad de rutas que salen de un lugar.
 *
 * @param g    [in] Grafo que contiene el lugar.
 * @param from [in] Índice del lugar de origen.
 * @return Cantidad de rutas salientes, o 0 si el índice es inválido.
 */
int graph_edge_count(const Graph *g, int from);

/**
 * @brief Verifica si existe una ruta con el nombre indicado.
 *
 * Recorre todas las listas de aristas del grafo buscando una ruta cuyo nombre
 * coincida exactamente con el parámetro.
 *
 * @param g    [in] Grafo en el que se busca.
 * @param name [in] Nombre de la ruta a buscar.
 * @return 1 si la ruta existe, 0 si no existe o hay error.
 */
int graph_find_route(const Graph *g, const char *name);

#endif
