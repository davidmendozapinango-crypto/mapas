/**
 * @file cli.h
 * @brief Interfaz de la interfaz de línea de comandos (CLI) del programa.
 *
 * Este módulo coordina la interacción con el usuario: solicita archivos de
 * rutas y clima, valida los lugares de origen y destino, y muestra los
 * resultados. También permite ejecutar la CLI con flujos de entrada y salida
 * personalizados, lo cual facilita las pruebas automatizadas.
 */

#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include "graph.h"
#include "weather.h"

/**
 * @brief Ejecuta la interfaz de línea de comandos con stdin y stdout.
 *
 * Punto de entrada estándar de la aplicación interactiva. Lee los archivos,
 * recibe las consultas del usuario y muestra los resultados por pantalla.
 *
 * @return 0 cuando la ejecución termina de forma normal.
 */
int cli_run(void);

/**
 * @brief Ejecuta la interfaz de línea de comandos usando flujos personalizados.
 *
 * Esta versión es útil para pruebas automatizadas, ya que permite redirigir
 * la entrada y la salida a archivos o buffers en memoria en lugar de usar la
 * consola real.
 *
 * @param in  [in]  Flujo de entrada (por ejemplo, stdin o un archivo).
 * @param out [in]  Flujo de salida (por ejemplo, stdout o un archivo).
 * @return 0 cuando la ejecución termina de forma normal.
 */
int cli_run_with_streams(FILE *in, FILE *out);

/**
 * @brief Muestra un prompt y lee una línea desde un flujo de entrada.
 *
 * La función elimina el carácter de nueva línea final y el retorno de carro
 * si están presentes, dejando en el búfer solo el texto introducido por el
 * usuario.
 *
 * @param prompt [in]  Texto del prompt a mostrar (puede ser NULL).
 * @param in     [in]  Flujo desde el que se lee.
 * @param out    [in]  Flujo donde se escribe el prompt.
 * @param buf    [out] Búfer donde se almacena la línea leída.
 * @param size   [in]  Tamaño máximo del búfer, incluyendo el carácter nulo.
 * @return 1 si se leyó una línea, 0 si se alcanzó el fin de archivo o hubo error.
 */
int cli_prompt_line(const char *prompt, FILE *in, FILE *out, char *buf, size_t size);

/**
 * @brief Carga un archivo de rutas en un grafo.
 *
 * Si el grafo apuntado por g ya contenía datos, se libera antes de cargar el
 * nuevo archivo. En caso de error, se escribe un mensaje descriptivo en el
 * búfer de error y se libera cualquier grafo parcial creado.
 *
 * @param g        [in,out] Puntero al puntero del grafo a cargar.
 * @param filename [in]     Ruta del archivo de rutas.
 * @param error    [out]    Búfer donde escribir el mensaje de error.
 * @param err_size [in]     Tamaño del búfer de error.
 * @return 1 si el archivo se cargó correctamente, 0 en caso contrario.
 */
int cli_load_route_file(Graph **g, const char *filename, char *error, size_t err_size);

/**
 * @brief Carga un archivo de condiciones climáticas.
 *
 * Si el filename es una cadena vacía, se crea un mapa climático vacío. Esto
 * permite al usuario omitir el archivo de clima y usar los costos base.
 *
 * @param wm       [in,out] Puntero al puntero del mapa climático a cargar.
 * @param filename [in]     Ruta del archivo de clima, o cadena vacía para omitir.
 * @param error    [out]    Búfer donde escribir el mensaje de error.
 * @param err_size [in]     Tamaño del búfer de error.
 * @return 1 si se cargó correctamente o se omitió de forma válida, 0 en error.
 */
int cli_load_weather_file(WeatherMap **wm, const char *filename, char *error, size_t err_size);

/**
 * @brief Valida que el origen y el destino sean lugares distintos del grafo.
 *
 * Verifica que ambos nombres existan en el grafo cargado y que no sean
 * iguales. Si alguna validación falla, escribe un mensaje explicativo en el
 * búfer de error.
 *
 * @param g      [in]  Grafo con los lugares cargados.
 * @param origin [in]  Nombre del lugar de origen.
 * @param dest   [in]  Nombre del lugar de destino.
 * @param error  [out] Búfer donde escribir el mensaje de error.
 * @param err_size [in] Tamaño del búfer de error.
 * @return 1 si origen y destino son válidos y distintos, 0 en caso contrario.
 */
int cli_validate_origin_destination(const Graph *g, const char *origin, const char *dest,
                                    char *error, size_t err_size);

/**
 * @brief Calcula y muestra las rutas óptimas y alternativas para un par origen-destino.
 *
 * Para cada modo de transporte, invoca el algoritmo de Yen para obtener la
 * mejor y la segunda mejor ruta, y luego imprime los resultados formateados
 * en el flujo de salida indicado.
 *
 * @param out    [in]  Flujo donde se escriben los resultados.
 * @param g      [in]  Grafo con el mapa cargado.
 * @param wm     [in]  Mapa climático (puede ser NULL).
 * @param origin [in]  Nombre del lugar de origen.
 * @param dest   [in]  Nombre del lugar de destino.
 */
void cli_compute_and_output(FILE *out, const Graph *g, const WeatherMap *wm,
                            const char *origin, const char *dest);

/**
 * @brief Interpreta la primera letra de una acción del usuario.
 *
 * Convierte la primera letra de la entrada a minúscula y devuelve un código
 * numérico según la acción elegida: nueva ruta, cambiar archivos o salir.
 *
 * @param input [in] Cadena introducida por el usuario.
 * @return 0 para nueva ruta, 1 para cambiar archivos, 2 para salir, -1 si es inválida.
 */
int cli_parse_action(const char *input);

#endif
