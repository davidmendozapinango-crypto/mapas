/**
 * @file types.h
 * @brief Definición de los tipos de datos principales del sistema de rutas.
 *
 * Este archivo centraliza todas las estructuras y enumeraciones que representan
 * los conceptos del dominio del problema: lugares, rutas, caminos, modos de
 * transporte y códigos de error. Separar los tipos en un encabezado propio
 * permite que cualquier otro módulo los use sin depender de implementaciones
 * concretas.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

/**
 * @brief Modos de transporte disponibles para calcular rutas.
 *
 * Cada modo tiene asociado un costo diferente en cada arista. La enumeración
 * también define MODE_COUNT, que se utiliza para dimensionar arreglos de
 * costos y para iterar sobre todos los modos.
 */
typedef enum {
    MODE_WALK = 0, /**< Desplazamiento a pie. */
    MODE_BUS = 1,  /**< Desplazamiento en autobús. */
    MODE_CAR = 2,  /**< Desplazamiento en carro particular. */
    MODE_COUNT = 3 /**< Número total de modos de transporte. */
} TransportMode;

/**
 * @brief Códigos de error utilizados por distintos módulos del programa.
 *
 * Estos valores permiten comunicar de forma estandarizada qué falló durante
 * la carga de archivos, la validación de entradas o el cálculo de rutas.
 */
typedef enum {
    OK = 0,              /**< Operación exitosa, no hay error. */
    ERR_FILE_NOT_FOUND,  /**< No se pudo encontrar o abrir un archivo. */
    ERR_INVALID_SYNTAX,  /**< El archivo tiene un formato incorrecto. */
    ERR_UNKNOWN_PLACE,   /**< Se hace referencia a un lugar que no existe. */
    ERR_NO_ROUTE,        /**< No existe una ruta entre el origen y el destino. */
    ERR_MEMORY,          /**< Falló una reserva de memoria dinámica. */
    ERR_INVALID_INPUT    /**< El usuario ingresó datos no válidos. */
} ErrorCode;

/**
 * @brief Representación de un lugar (vértice) dentro del grafo.
 *
 * Cada lugar tiene un nombre legible por humanos y un índice numérico que lo
 * identifica dentro del arreglo de lugares del grafo.
 */
typedef struct {
    char *name;  /**< Nombre del lugar, asignado en memoria dinámica. */
    int index;   /**< Índice del lugar dentro del arreglo del grafo. */
} Place;

/**
 * @brief Representación de una ruta directa entre dos lugares (arista).
 *
 * Una ruta conecta un lugar de origen con un lugar de destino y almacena un
 * costo para cada modo de transporte. Un costo menor o igual a cero indica que
 * el modo no está disponible para esa ruta.
 */
typedef struct {
    char *name;              /**< Nombre identificador de la ruta. */
    int from;                /**< Índice del lugar de origen. */
    int to;                  /**< Índice del lugar de destino. */
    double costs[MODE_COUNT]; /**< Costos para a pie, bus y carro. */
} Route;

/**
 * @brief Camino resultante de un algoritmo de búsqueda de rutas.
 *
 * Almacena la secuencia de índices de lugares que conforman la ruta, la
 * cantidad de lugares visitados, el costo total acumulado y el modo de
 * transporte con el que se calculó.
 */
typedef struct {
    int *places;       /**< Arreglo dinámico con los índices de los lugares. */
    int place_count;   /**< Cantidad de lugares en el arreglo. */
    double total_cost; /**< Costo total del camino. */
    TransportMode mode; /**< Modo de transporte utilizado. */
} Path;

/**
 * @brief Resultado de buscar la mejor y la segunda mejor ruta.
 *
 * El algoritmo de Yen utiliza esta estructura para devolver tanto la ruta
 * óptima como una ruta alternativa, si existen. Los indicadores has_best y
 * has_second permiten saber cuáles de los dos campos son válidos.
 */
typedef struct {
    Path best;    /**< Mejor ruta encontrada. */
    Path second;  /**< Segunda mejor ruta encontrada. */
    int has_best;    /**< Indica si el campo best contiene una ruta válida. */
    int has_second;  /**< Indica si el campo second contiene una ruta válida. */
} PathResult;

#endif
