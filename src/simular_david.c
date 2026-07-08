#include "TDA_graph_proy.h"

int main() {
    Grafo_Mapa * miMapa = crearGrafo();

    // 1. Simulación de David cargando las rutas del archivo
    Costos costosRutaA = {2.5, 0.0, 1.0}; // P:2.5; B:0; C:1
    agregarRuta(miMapa, "RutaA", "Heladeria", "Ferreteria", costosRutaA);

    Costos costosRutaB = {2.5, 13.0, 10.0}; // P:2.5; B:13; C:10
    agregarRuta(miMapa, "RutaB", "Heladeria", "Ferreteria", costosRutaB);

    // En este punto, miMapa->V será 2. 
    // "Heladeria" tendrá el índice 0 y "Ferreteria" el índice 1.
    // miMapa->numRutas[0] será 2 (porque RutaA y RutaB salen de Heladería).

    // 2. Simulación de David aplicando el archivo de clima opcional
    Costos climaRutaB = {0.0, 1.0, 1.3}; // P:0; B:1; C:1.3
    aplicarClimaARuta(miMapa, "RutaB", climaRutaB);

    // Ahora, si revisas miMapa->adj[0][1] (la RutaB), verás que su costoActual en pie pasó a ser 0.0

    // 3. Liberar memoria al terminar
    free(miMapa);
    return 0;
}