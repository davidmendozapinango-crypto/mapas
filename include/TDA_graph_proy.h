/**
 * @file TDA_graph_proy.h
 * @brief Definición del TDA Grafo para el sistema de optimización de rutas de viaje.
 * @author Albert & David
 * @date 2026
 *
 * Este archivo define una representación alternativa de grafo basada en una
 * matriz de adyacencia con costos para tres modalidades de transporte: a pie,
 * en bus y en carro. Está pensado para experimentos y simulaciones del
 * proyecto, con límites fijos de vértices y rutas por vértice.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/** @brief Número máximo de vértices (lugares) permitidos en el mapa. */
#define MAX_VERTICES 30

/** @brief Capacidad máxima de rutas de salida por cada vértice (contando aristas paralelas). */
#define MAX_RUTAS_POR_VERTICE 10

/** @brief Tamaño máximo para las cadenas de texto de los nombres. */
#define MAX_NOM 50

/**
 * @struct Costos
 * @brief Pesos de las tres modalidades de transporte.
 *
 * Un valor de 0.0 indica que la ruta no es transitable por esa modalidad.
 */
typedef struct Costos {
    double pie;   /**< Tiempo estimado de tránsito a pie. */
    double bus;   /**< Tiempo estimado de tránsito en bus. */
    double carro; /**< Tiempo estimado de tránsito en carro. */
} Costos;

/**
 * @struct Ruta_arista
 * @brief Representación de una ruta (arista dirigida) dentro del grafo.
 *
 * Almacena todos los datos relevantes de la ruta: nombre, destino, costos
 * base cargados desde el archivo de configuración y costos afectados por el
 * clima.
 */
typedef struct Ruta_arista {
    char nombre_ruta[MAX_NOM]; /**< Identificador de la calle o ruta. */
    int destino;               /**< Índice numérico del vértice de llegada en el arreglo del grafo. */
    Costos costos_base;        /**< Costos de viaje originales cargados desde el archivo de configuración. */
    Costos costos_clima;       /**< Costos vigentes potenciados por los coeficientes climáticos actuales. */
} Ruta_arista;

/**
 * @struct Nodo_lugar
 * @brief Representación de un sitio (vértice) en el mapa de la ciudad.
 */
typedef struct Nodo_lugar {
    char nombre_lugar[MAX_NOM]; /**< Nombre único del sitio. */
} Nodo_lugar;

/**
 * @struct Grafo_Mapa
 * @brief Matriz de adyacencia utilizada para representar las conexiones del mapa.
 *
 * La matriz adj almacena las rutas salientes de cada vértice. La fila i
 * corresponde al vértice i, y cada columna j representa una de las rutas
 * salientes de ese vértice. El arreglo num_rutas indica cuántas rutas válidas
 * tiene cada vértice.
 */
typedef struct Grafo_Mapa {
    int V;                                        /**< Cantidad de lugares en el mapa. */
    Nodo_lugar vertices[MAX_VERTICES];            /**< Arreglo indexado para traducir IDs numéricos a nombres de lugares. */
    Ruta_arista adj[MAX_VERTICES][MAX_RUTAS_POR_VERTICE]; /**< Matriz de inventario de rutas. Fila: origen. Columna: espacio de guardado. */
    int num_rutas[MAX_VERTICES];                  /**< Contador del número de rutas válidas que salen de cada vértice. */
} Grafo_Mapa;
