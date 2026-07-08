/**
 * @file TDA_graph_proy.h
 * @brief Definición del TDA Grafo para el sitema de optimización de rutas de viaje.
 * @author Albert & David
 * @date 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/** @brief Número máximo de vértices(lugares) permitidos en el mapa */
#define MAX_VERTICES 30
/** @brief Capacidad máxima de rutas de salida por cada vértice(contando aristas paralelas)*/
#define MAX_RUTAS_POR_VERTICE 10
/** @brief Tamaño máximo para las cadenas de texto de los nombre*/
#define MAX_NOM 50

/**
 * @struct Costos
 * @brief estructura pesos de las tres modalidades de transporte
 * Un 0.0 indica que la ruta no es transitable 
 */
typedef struct Costos {
    double pie; /**<tiempo estimado de tránsito a pie*/
    double bus;  /**< " a bus*/
    double carro; /**< " en carro*/
} Costos;

/**
 * @struct Ruta_arista
 * @brief Representación de una ruta(arista dirigida) dentro del grafo
 * Almacena todos los datos relevantes a la ruta
 */
typedef struct Ruta_arista {
    char nombre_ruta[MAX_NOM]; /**< Identificador de la calle o ruta*/
    int destino; /**< Índice numérico del vértice de llegada en el arreglo del grafo*/
    Costos costos_base; /**< Costos de viaje originales cargados desde el archivo de configuración*/
    Costos costos_clima; /**< Costos vigentes potendiados por los coeficientes climáticos actuales*/
} Ruta_arista;

/**
 * @struct Nodo_lugar
 * @brief Representación de un sitio(vértice) en el mapa de la ciudad
 */
typedef struct Nodo_lugar {
    char nombre_lugar[MAX_NOM]; /**< Nombre único del sitio*/
} Nodo_lugar;

/**
 * @struct Grafo_Mapa
 * @brief Matriz de Adyacencia utilizada para representar las conexiones de mapa
 */
typedef struct Grafo_Mapa {
    int V; /**< cantidad de lugares en el mapa*/
    Nodo_lugar vertices[MAX_VERTICES]; /**< Arreglo indexado para traducir IDs numéricos a nombres de lugare*/
    Ruta_arista adj[MAX_VERTICES][MAX_RUTAS_POR_VERTICE]; /**< Matriz de inventario de rutas. Fila: Origen. Columna: Espacio de guardado*/
    int num_rutas[MAX_VERTICES]; /**< Contador del número de rutas válidas que salen de cada vértice específico*/
} Grafo_Mapa;