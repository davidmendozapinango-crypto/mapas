/**
 * @file weather.h
 * @brief Interfaz para la gestión de condiciones climáticas sobre rutas.
 *
 * El clima se modela como un conjunto de multiplicadores asociados a rutas
 * específicas. Cada multiplicador ajusta el costo base de una ruta para un
 * modo de transporte dado. Un multiplicador de 1.0 indica condiciones
 * normales, mientras que valores mayores o menores representan condiciones
 * más lentas o más rápidas, respectivamente.
 */

#ifndef WEATHER_H
#define WEATHER_H

#include "types.h"
#include "graph.h"

/**
 * @brief Condición climática aplicada a una ruta específica.
 *
 * Almacena el nombre de la ruta afectada y tres multiplicadores, uno por
 * cada modo de transporte.
 */
typedef struct {
    char *route_name;            /**< Nombre de la ruta afectada. */
    double multipliers[MODE_COUNT]; /**< Multiplicadores por modo de transporte. */
} WeatherCondition;

/**
 * @brief Conjunto de condiciones climáticas cargadas desde un archivo.
 *
 * Mantiene un arreglo dinámico de condiciones y su capacidad actual.
 */
typedef struct {
    WeatherCondition *conditions; /**< Arreglo dinámico de condiciones. */
    int count;                    /**< Cantidad de condiciones almacenadas. */
    int capacity;                 /**< Capacidad reservada del arreglo. */
} WeatherMap;

/**
 * @brief Crea un mapa climático vacío.
 *
 * Reserva memoria para el arreglo de condiciones. Si la capacidad inicial es
 * menor o igual a cero, se usa un valor por defecto.
 *
 * @param initial_capacity Capacidad inicial del arreglo.
 * @return Puntero al mapa climático, o NULL si falla la reserva.
 */
WeatherMap *weather_create(int initial_capacity);

/**
 * @brief Libera toda la memoria de un mapa climático.
 *
 * Libera los nombres de las rutas, el arreglo de condiciones y la estructura.
 *
 * @param wm [in,out] Mapa climático a destruir.
 */
void weather_destroy(WeatherMap *wm);

/**
 * @brief Agrega o actualiza una condición climática.
 *
 * Si la ruta ya existe en el mapa, se actualizan sus multiplicadores. Si no,
 * se crea una nueva entrada. El nombre de la ruta se copia a memoria dinámica.
 *
 * @param wm     [in,out] Mapa climático.
 * @param route_name [in] Nombre de la ruta afectada.
 * @param walk   [in] Multiplicador para ir a pie.
 * @param bus    [in] Multiplicador para ir en bus.
 * @param car    [in] Multiplicador para ir en carro.
 * @return 1 si se guardó correctamente, 0 en caso de error.
 */
int weather_add(WeatherMap *wm, const char *route_name, double walk, double bus, double car);

/**
 * @brief Calcula el costo efectivo de una ruta bajo ciertas condiciones climáticas.
 *
 * Multiplica el costo base de la ruta para el modo indicado por el
 * multiplicador correspondiente del mapa climático. Si no hay condición para
 * la ruta, se usa el costo base sin cambios. Un costo base o multiplicador
 * menor o igual a cero se interpreta como ruta intransitable.
 *
 * @param wm   [in] Mapa climático (puede ser NULL).
 * @param route [in] Ruta cuyo costo se desea calcular.
 * @param mode [in] Modo de transporte.
 * @return Costo efectivo de la ruta, o INF_COST si es intransitable.
 */
double weather_effective_cost(const WeatherMap *wm, const Route *route, TransportMode mode);

/**
 * @brief Verifica si una ruta tiene una condición climática registrada.
 *
 * @param wm         [in] Mapa climático.
 * @param route_name [in] Nombre de la ruta a buscar.
 * @return 1 si la ruta tiene condiciones, 0 si no o si hay error.
 */
int weather_has_route(const WeatherMap *wm, const char *route_name);

/**
 * @brief Advertencia sobre rutas climáticas desconocidas en el grafo.
 *
 * Recorre el mapa climático y, para cada ruta que no exista en el grafo,
 * imprime una advertencia en la salida de error estándar.
 *
 * @param wm [in] Mapa climático.
 * @param g  [in] Grafo con las rutas válidas.
 */
void weather_report_unknown_routes(const WeatherMap *wm, const Graph *g);

#endif
