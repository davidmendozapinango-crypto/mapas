/**
 * @file weather.c
 * @brief Implementación de la gestión de condiciones climáticas.
 *
 * Este archivo administra las condiciones climáticas asociadas a rutas
 * específicas. Cada condición contiene multiplicadores para los tres modos
 * de transporte. El costo efectivo de una ruta se calcula multiplicando el
 * costo base por el multiplicador correspondiente.
 */

#include "weather.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** @brief Valor que representa un costo infinito o una ruta intransitable. */
#define INF_COST 1e300

/**
 * @brief Crea un mapa climático vacío.
 *
 * Reserva memoria para la estructura principal y para el arreglo de
 * condiciones. Si la capacidad inicial es menor o igual a cero, se usa el
 * valor por defecto 16.
 *
 * @param initial_capacity Capacidad inicial del arreglo de condiciones.
 * @return Puntero al mapa climático, o NULL si falla la reserva.
 */
WeatherMap *weather_create(int initial_capacity) {
    WeatherMap *wm = malloc(sizeof(WeatherMap));
    if (!wm) return NULL;

    wm->count = 0;
    wm->capacity = initial_capacity > 0 ? initial_capacity : 16;
    wm->conditions = malloc(sizeof(WeatherCondition) * wm->capacity);
    if (!wm->conditions) {
        free(wm);
        return NULL;
    }

    return wm;
}

/**
 * @brief Libera toda la memoria de un mapa climático.
 *
 * Recorre el arreglo de condiciones liberando el nombre de cada ruta, luego
 * libera el arreglo de condiciones y la estructura principal.
 *
 * @param wm [in,out] Mapa climático a destruir.
 */
void weather_destroy(WeatherMap *wm) {
    if (!wm) return;

    for (int i = 0; i < wm->count; i++) {
        free(wm->conditions[i].route_name);
    }
    free(wm->conditions);
    free(wm);
}

/**
 * @brief Busca el índice de una ruta dentro del mapa climático.
 *
 * Realiza una búsqueda lineal comparando el nombre de la ruta.
 *
 * @param wm         [in] Mapa climático.
 * @param route_name [in] Nombre de la ruta a buscar.
 * @return Índice de la condición, o -1 si no se encuentra.
 */
static int find_index(const WeatherMap *wm, const char *route_name) {
    for (int i = 0; i < wm->count; i++) {
        if (strcmp(wm->conditions[i].route_name, route_name) == 0) return i;
    }
    return -1;
}

/**
 * @brief Agrega o actualiza una condición climática en el mapa.
 *
 * Si la ruta ya existe, se actualizan sus multiplicadores. Si no existe, se
 * crea una nueva entrada, copiando el nombre de la ruta a memoria dinámica.
 * El arreglo de condiciones se redimensiona automáticamente cuando es
 * necesario.
 *
 * @param wm         [in,out] Mapa climático.
 * @param route_name [in]     Nombre de la ruta afectada.
 * @param walk       [in]     Multiplicador para ir a pie.
 * @param bus        [in]     Multiplicador para ir en bus.
 * @param car        [in]     Multiplicador para ir en carro.
 * @return 1 si se guardó correctamente, 0 en caso de error.
 */
int weather_add(WeatherMap *wm, const char *route_name, double walk, double bus, double car) {
    if (!wm || !route_name) return 0;

    int idx = find_index(wm, route_name);
    if (idx < 0) {
        // La ruta no existe, hay que crear una nueva entrada.
        if (wm->count >= wm->capacity) {
            int new_cap = wm->capacity * 2;
            WeatherCondition *new_conds = realloc(wm->conditions, sizeof(WeatherCondition) * new_cap);
            if (!new_conds) return 0;
            wm->conditions = new_conds;
            wm->capacity = new_cap;
        }

        idx = wm->count++;
        wm->conditions[idx].route_name = xstrdup(route_name);
        if (!wm->conditions[idx].route_name) return 0;
    }

    wm->conditions[idx].multipliers[MODE_WALK] = walk;
    wm->conditions[idx].multipliers[MODE_BUS] = bus;
    wm->conditions[idx].multipliers[MODE_CAR] = car;
    return 1;
}

/**
 * @brief Calcula el costo efectivo de una ruta bajo ciertas condiciones climáticas.
 *
 * El costo base de la ruta para el modo indicado se multiplica por el
 * multiplicador correspondiente del mapa climático. Si la ruta no tiene una
 * condición asignada, el multiplicador es 1.0. Si el costo base o el
 * multiplicador es menor o igual a cero, la ruta se considera intransitable.
 *
 * @param wm   [in] Mapa climático (puede ser NULL).
 * @param route [in] Ruta cuyo costo se desea calcular.
 * @param mode [in] Modo de transporte.
 * @return Costo efectivo de la ruta, o INF_COST si es intransitable.
 */
double weather_effective_cost(const WeatherMap *wm, const Route *route, TransportMode mode) {
    if (!route) return INF_COST;

    double base = route->costs[mode];
    if (base <= 0.0) return INF_COST;

    double mult = 1.0;
    if (wm) {
        int idx = find_index(wm, route->name);
        if (idx >= 0) {
            mult = wm->conditions[idx].multipliers[mode];
            if (mult <= 0.0) return INF_COST;
        }
    }

    return base * mult;
}

/**
 * @brief Verifica si una ruta tiene una condición climática registrada.
 *
 * @param wm         [in] Mapa climático.
 * @param route_name [in] Nombre de la ruta a buscar.
 * @return 1 si la ruta tiene condiciones, 0 si no o si hay error.
 */
int weather_has_route(const WeatherMap *wm, const char *route_name) {
    if (!wm || !route_name) return 0;
    return find_index(wm, route_name) >= 0;
}

/**
 * @brief Advertencia sobre rutas climáticas desconocidas en el grafo.
 *
 * Recorre todas las condiciones climáticas cargadas. Si alguna hace referencia
 * a una ruta que no existe en el grafo, se imprime una advertencia en la
 * salida de error estándar indicando que esa condición será ignorada.
 *
 * @param wm [in] Mapa climático.
 * @param g  [in] Grafo con las rutas válidas.
 */
void weather_report_unknown_routes(const WeatherMap *wm, const Graph *g) {
    if (!wm || !g) return;

    for (int i = 0; i < wm->count; i++) {
        if (!graph_find_route(g, wm->conditions[i].route_name)) {
            fprintf(stderr, "Advertencia: la linea %d del clima referencia la ruta desconocida \"%s\" y sera ignorada.\n",
                    i + 1, wm->conditions[i].route_name);
        }
    }
}
