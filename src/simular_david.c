/**
 * @file simular_david.c
 * @brief Simulación de uso del TDA Grafo del proyecto.
 *
 * Este archivo muestra cómo crear un grafo, agregar rutas entre lugares y
 * aplicar coeficientes climáticos a una ruta específica. Es un ejemplo
 * ilustrativo para entender el funcionamiento básico del TDA Grafo.
 */

#include "TDA_graph_proy.h"

/**
 * @brief Punto de entrada de la simulación.
 *
 * Crea un grafo, registra dos rutas entre "Heladeria" y "Ferreteria" con
 * diferentes costos, aplica un coeficiente climático a una de ellas y luego
 * libera la memoria del grafo.
 *
 * @return 0 al finalizar la simulación.
 */
int main() {
    Grafo_Mapa *miMapa = crearGrafo();

    // 1. Simulación de David cargando las rutas del archivo.
    Costos costosRutaA = {2.5, 0.0, 1.0}; // P:2.5; B:0; C:1
    agregarRuta(miMapa, "RutaA", "Heladeria", "Ferreteria", costosRutaA);

    Costos costosRutaB = {2.5, 13.0, 10.0}; // P:2.5; B:13; C:10
    agregarRuta(miMapa, "RutaB", "Heladeria", "Ferreteria", costosRutaB);

    // En este punto, miMapa->V será 2.
    // "Heladeria" tendrá el índice 0 y "Ferreteria" el índice 1.
    // miMapa->numRutas[0] será 2 (porque RutaA y RutaB salen de Heladeria).

    // 2. Simulación de David aplicando el archivo de clima opcional.
    Costos climaRutaB = {0.0, 1.0, 1.3}; // P:0; B:1; C:1.3
    aplicarClimaARuta(miMapa, "RutaB", climaRutaB);

    // Ahora, si revisas miMapa->adj[0][1] (la RutaB), veras que su
    // costoActual en pie paso a ser 0.0.

    // 3. Liberar memoria al terminar.
    free(miMapa);
    return 0;
}
