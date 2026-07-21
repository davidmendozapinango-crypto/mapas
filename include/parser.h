/**
 * @file parser.h
 * @brief Interfaz para la carga de archivos de rutas y clima.
 *
 * Este módulo se encarga de leer los archivos de texto que definen los
 * lugares, las rutas y los multiplicadores climáticos, y de poblar con ellos
 * las estructuras de grafo y mapa climático.
 */

#ifndef PARSER_H
#define PARSER_H

#include "graph.h"
#include "weather.h"

/**
 * @brief Lee un archivo de rutas y lo carga en un grafo.
 *
 * El archivo debe contener primero una sección de lugares encabezada por la
 * palabra "Lugares", seguida de una sección de rutas con el formato:
 * nombre_ruta -> origen:destino = P:costo; B:costo; C:costo.
 *
 * @param filename  [in]  Ruta del archivo a leer.
 * @param g         [in,out] Grafo que se poblará con los datos.
 * @param error_msg [out] Búfer para el mensaje de error.
 * @param error_size [in] Tamaño del búfer de error.
 * @return 1 si el archivo se cargó correctamente, 0 si hubo un error.
 */
int parse_route_file(const char *filename, Graph *g, char *error_msg, size_t error_size);

/**
 * @brief Lee un archivo de condiciones climáticas y lo carga en un mapa.
 *
 * Cada línea válida debe tener el formato:
 * nombre_ruta = P:multiplicador; B:multiplicador; C:multiplicador.
 * Las líneas con formato incorrecto se ignoran con una advertencia.
 *
 * @param filename  [in]  Ruta del archivo a leer.
 * @param wm        [in,out] Mapa climático que se poblará.
 * @param error_msg [out] Búfer para el mensaje de error.
 * @param error_size [in] Tamaño del búfer de error.
 * @return 1 si el archivo se cargó correctamente.
 */
int parse_weather_file(const char *filename, WeatherMap *wm, char *error_msg, size_t error_size);

#endif
