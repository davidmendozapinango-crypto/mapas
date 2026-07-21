/**
 * @file main.c
 * @brief Punto de entrada principal de la aplicación Mapas.
 *
 * Este archivo contiene la función main, que simplemente delega la ejecución
 * de la interfaz de línea de comandos (CLI) en la función cli_run. Separar
 * el punto de entrada del resto de la lógica facilita las pruebas, ya que
 * los tests pueden invocar directamente las funciones de la CLI sin depender
 * de la función main.
 */

#include "cli.h"

/**
 * @brief Inicia la aplicación Mapas.
 *
 * Llama a cli_run para arrancar la interfaz interactiva que solicita archivos
 * de rutas y clima, recibe consultas de origen y destino, y muestra los
 * resultados por pantalla.
 *
 * @return 0 si la aplicación termina correctamente.
 */
int main(void)
{
    return cli_run();
}
