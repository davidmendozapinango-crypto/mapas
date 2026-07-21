/**
 * @file graph.c
 * @brief Implementación de la gestión de grafos de rutas.
 *
 * Este archivo contiene las funciones para crear, destruir y modificar el
 * grafo dirigido que representa el mapa de la ciudad. Cada lugar se almacena
 * como un vértice y cada ruta como una arista dirigida con costos para los
 * tres modos de transporte.
 */

#include "graph.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/**
 * @brief Valida que un nombre solo contenga letras, dígitos o guiones bajos.
 *
 * Los nombres de lugares y rutas deben ser cadenas no vacías compuestas por
 * caracteres alfanuméricos o guiones bajos. Esto evita que espacios o símbolos
 * extraños generen problemas en el archivo de configuración.
 *
 * @param name [in] Cadena a validar.
 * @return 1 si el nombre es válido, 0 en caso contrario.
 */
static int is_valid_name(const char *name) {
    if (name == NULL || name[0] == '\0') return 0;
    for (size_t i = 0; name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)name[i]) && name[i] != '_') return 0;
    }
    return 1;
}

/**
 * @brief Crea un grafo vacío con una capacidad inicial.
 *
 * Reserva memoria para la estructura principal, para el arreglo de lugares y
 * para el arreglo de listas de aristas. Inicializa cada lista de aristas con
 * conteo y capacidad en cero. Si alguna reserva falla, se libera todo lo
 * reservado hasta ese momento y se devuelve NULL.
 *
 * @param initial_capacity Capacidad inicial deseada para el arreglo de lugares.
 * @return Puntero al grafo creado, o NULL si falla alguna reserva de memoria.
 */
Graph *graph_create(int initial_capacity) {
    Graph *g = malloc(sizeof(Graph));
    if (!g) return NULL;

    g->place_count = 0;
    g->place_capacity = initial_capacity > 0 ? initial_capacity : 16;

    g->places = malloc(sizeof(Place) * g->place_capacity);
    if (!g->places) {
        free(g);
        return NULL;
    }

    g->edges = malloc(sizeof(EdgeList) * g->place_capacity);
    if (!g->edges) {
        free(g->places);
        free(g);
        return NULL;
    }

    for (int i = 0; i < g->place_capacity; i++) {
        g->edges[i].routes = NULL;
        g->edges[i].count = 0;
        g->edges[i].capacity = 0;
    }

    return g;
}

/**
 * @brief Libera toda la memoria asociada a un grafo.
 *
 * Recorre cada lugar para liberar el nombre que se copió dinámicamente. Luego
 * recorre cada lista de aristas, libera los nombres de las rutas y el arreglo
 * de rutas. Finalmente libera los arreglos de lugares y aristas, y la
 * estructura principal. Si el puntero es NULL, la función no realiza ninguna
 * acción.
 *
 * @param g [in,out] Grafo a destruir.
 */
void graph_destroy(Graph *g) {
    if (!g) return;

    for (int i = 0; i < g->place_count; i++) {
        free(g->places[i].name);
    }
    free(g->places);

    for (int i = 0; i < g->place_capacity; i++) {
        if (g->edges[i].routes) {
            for (int j = 0; j < g->edges[i].count; j++) {
                free(g->edges[i].routes[j].name);
            }
            free(g->edges[i].routes);
        }
    }
    free(g->edges);
    free(g);
}

/**
 * @brief Duplica la capacidad de los arreglos internos del grafo.
 *
 * Cuando se agregan más lugares y el arreglo de lugares está lleno, esta
 * función reserva nuevos arreglos del doble de tamaño, copia los datos
 * existentes e inicializa las nuevas entradas de la lista de aristas.
 *
 * @param g [in,out] Grafo cuya capacidad se desea ampliar.
 * @return 1 si la ampliación fue exitosa, 0 si falló alguna reserva.
 */
static int ensure_place_capacity(Graph *g) {
    if (g->place_count < g->place_capacity) return 1;

    int new_cap = g->place_capacity * 2;

    Place *new_places = realloc(g->places, sizeof(Place) * new_cap);
    if (!new_places) return 0;
    g->places = new_places;

    EdgeList *new_edges = realloc(g->edges, sizeof(EdgeList) * new_cap);
    if (!new_edges) return 0;
    g->edges = new_edges;

    for (int i = g->place_capacity; i < new_cap; i++) {
        g->edges[i].routes = NULL;
        g->edges[i].count = 0;
        g->edges[i].capacity = 0;
    }

    g->place_capacity = new_cap;
    return 1;
}

/**
 * @brief Busca el índice de un lugar por su nombre.
 *
 * Recorre el arreglo de lugares comparando el nombre buscado con cada nombre
 * almacenado. La comparación es sensible a mayúsculas y minúsculas.
 *
 * @param g    [in] Grafo en el que se busca.
 * @param name [in] Nombre del lugar a buscar.
 * @return Índice del lugar si se encuentra, -1 si no se encuentra o hay error.
 */
int graph_find_place(const Graph *g, const char *name) {
    if (!g || !name) return -1;
    for (int i = 0; i < g->place_count; i++) {
        if (strcmp(g->places[i].name, name) == 0) return i;
    }
    return -1;
}

/**
 * @brief Agrega un nuevo lugar al grafo.
 *
 * Si el lugar ya existe, devuelve su índice actual. Si no existe, amplía la
 * capacidad si es necesario, copia el nombre del lugar con xstrdup, asigna
 * el índice correspondiente e incrementa el contador de lugares.
 *
 * @param g    [in,out] Grafo al que se agrega el lugar.
 * @param name [in]     Nombre del lugar a agregar.
 * @return Índice del lugar dentro del grafo, o -1 si ocurre un error.
 */
int graph_add_place(Graph *g, const char *name) {
    if (!g || !name || !is_valid_name(name)) return -1;

    int idx = graph_find_place(g, name);
    if (idx >= 0) return idx;

    if (!ensure_place_capacity(g)) return -1;

    idx = g->place_count;
    g->places[idx].name = xstrdup(name);
    if (!g->places[idx].name) return -1;
    g->places[idx].index = idx;
    g->place_count++;

    return idx;
}

/**
 * @brief Agrega una ruta dirigida entre dos lugares del grafo.
 *
 * Busca los índices de los lugares de origen y destino. Si alguno no existe,
 * la función falla. Luego asegura que la lista de aristas del origen tenga
 * capacidad disponible, copia el nombre de la ruta y guarda los costos para
 * cada modo de transporte.
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
                    double walk_cost, double bus_cost, double car_cost) {
    if (!g || !name || !from || !to) return 0;

    int from_idx = graph_find_place(g, from);
    int to_idx = graph_find_place(g, to);
    if (from_idx < 0 || to_idx < 0) return 0;

    EdgeList *list = &g->edges[from_idx];
    if (list->count >= list->capacity) {
        int new_cap = list->capacity == 0 ? 4 : list->capacity * 2;
        Route *new_routes = realloc(list->routes, sizeof(Route) * new_cap);
        if (!new_routes) return 0;
        list->routes = new_routes;
        list->capacity = new_cap;
    }

    Route *r = &list->routes[list->count];
    r->name = xstrdup(name);
    if (!r->name) return 0;
    r->from = from_idx;
    r->to = to_idx;
    r->costs[MODE_WALK] = walk_cost;
    r->costs[MODE_BUS] = bus_cost;
    r->costs[MODE_CAR] = car_cost;
    list->count++;

    return 1;
}

/**
 * @brief Verifica si existe una ruta con el nombre indicado.
 *
 * Recorre todas las listas de aristas del grafo comparando el nombre de cada
 * ruta con el nombre buscado.
 *
 * @param g    [in] Grafo en el que se busca.
 * @param name [in] Nombre de la ruta a buscar.
 * @return 1 si la ruta existe, 0 si no existe o hay error.
 */
int graph_find_route(const Graph *g, const char *name) {
    if (!g || !name) return 0;
    for (int i = 0; i < g->place_count; i++) {
        for (int j = 0; j < g->edges[i].count; j++) {
            if (strcmp(g->edges[i].routes[j].name, name) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Obtiene una ruta específica desde la lista de aristas de un lugar.
 *
 * Permite acceder a una arista por su posición dentro de la lista de rutas
 * salientes del lugar origen. Es útil para iterar sin exponer la estructura
 * interna del grafo.
 *
 * @param g    [in] Grafo que contiene la ruta.
 * @param from [in] Índice del lugar de origen.
 * @param idx  [in] Posición de la ruta dentro de la lista de aristas.
 * @return Puntero a la ruta solicitada, o NULL si los índices son inválidos.
 */
const Route *graph_get_edge(const Graph *g, int from, int idx) {
    if (!g || from < 0 || from >= g->place_count) return NULL;
    if (idx < 0 || idx >= g->edges[from].count) return NULL;
    return &g->edges[from].routes[idx];
}

/**
 * @brief Devuelve la cantidad de rutas que salen de un lugar.
 *
 * @param g    [in] Grafo que contiene el lugar.
 * @param from [in] Índice del lugar de origen.
 * @return Cantidad de rutas salientes, o 0 si el índice es inválido.
 */
int graph_edge_count(const Graph *g, int from) {
    if (!g || from < 0 || from >= g->place_count) return 0;
    return g->edges[from].count;
}
