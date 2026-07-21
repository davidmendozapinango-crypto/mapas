/**
 * @file tda_grafos.c
 * @brief Laboratorio de grafos dirigidos con algoritmos de Kahn y Kosaraju.
 *
 * Este archivo implementa un grafo dirigido usando una matriz de adyacencia
 * y demuestra dos algoritmos clásicos:
 * - Orden topológico con el algoritmo de Kahn.
 * - Componentes fuertemente conexas con el algoritmo de Kosaraju.
 *
 * El orden topológico organiza los vértices de un grafo dirigido acíclico de
 * modo que todas las aristas apuntan de un vértice anterior a uno posterior.
 * Kosaraju encuentra grupos de vértices donde cada vértice alcanza a todos los
 * demás del grupo siguiendo las aristas dirigidas.
 *
 * @author Albert Gonzalez
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/** @brief Número máximo de vértices soportados en esta representación. */
#define MAX_VERTICES 20

/**
 * @brief Grafo dirigido representado con una matriz de adyacencia.
 *
 * La matriz adj[i][j] vale 1 si existe una arista dirigida de i a j, y 0 en
 * caso contrario.
 */
typedef struct Graph {
    int V;                            /**< Cantidad de vértices del grafo. */
    int adj[MAX_VERTICES][MAX_VERTICES]; /**< Matriz de adyacencia. */
} Graph;

/**
 * @brief Crea un grafo con una cantidad dada de vértices.
 *
 * Reserva memoria para la estructura e inicializa la matriz de adyacencia con
 * ceros, indicando que inicialmente no hay aristas.
 *
 * @param vertices Cantidad de vértices del grafo.
 * @return Puntero al grafo creado.
 */
Graph* createGraph(int vertices) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->V = vertices;
    for (int i = 0; i < vertices; i++)
        for (int j = 0; j < vertices; j++)
            g->adj[i][j] = 0;
    return g;
}

/**
 * @brief Agrega una arista dirigida entre dos vértices.
 *
 * Solo agrega la arista si ambos vértices están dentro del rango válido del
 * grafo.
 *
 * @param g [in,out] Grafo al que se agrega la arista.
 * @param u [in]     Vértice de origen.
 * @param v [in]     Vértice de destino.
 */
void addEdge(Graph* g, int u, int v) {
    if (u < g->V && v < g->V)
        g->adj[u][v] = 1;
}

/**
 * @brief Calcula el orden topológico de un grafo usando el algoritmo de Kahn.
 *
 * El algoritmo de Kahn funciona en grafos dirigidos acíclicos. Primero calcula
 * el grado de entrada de cada vértice (cuántas aristas llegan a él). Luego
 * coloca en una cola todos los vértices con grado de entrada cero, es decir,
 * aquellos que no tienen dependencias previas. Extrae un vértice de la cola,
 * lo añade al orden topológico y reduce el grado de entrada de sus vecinos;
 * si algún vecino llega a grado cero, también se encola.
 *
 * Si al final no se han procesado todos los vértices, significa que el grafo
 * contiene al menos un ciclo y no tiene orden topológico.
 *
 * @param g [in,out] Grafo sobre el que se calcula el orden topológico.
 */
void kahnTopologicalSort(Graph* g) {
    int in_degree[MAX_VERTICES] = {0};

    // Calcular el grado de entrada de cada vértice.
    for (int i = 0; i < g->V; i++)
        for (int j = 0; j < g->V; j++)
            if (g->adj[i][j]) in_degree[j]++;

    int queue[MAX_VERTICES];
    int front = 0, rear = 0;

    // Encolar los vértices que no tienen dependencias previas.
    for (int i = 0; i < g->V; i++)
        if (in_degree[i] == 0) queue[rear++] = i;

    int top_order[MAX_VERTICES];
    int count = 0;

    printf("\n[KAHN] Iniciando procesamiento...\n");
    while (front < rear) {
        int u = queue[front++];
        top_order[count++] = u;
        printf("  -> Nodo [%d] liberado\n", u);

        // Reducir el grado de entrada de los vecinos de u.
        for (int v = 0; v < g->V; v++) {
            if (g->adj[u][v]) {
                in_degree[v]--;
                if (in_degree[v] == 0) queue[rear++] = v;
            }
        }
    }

    // Si no se procesaron todos los vértices, hay un ciclo.
    if (count != g->V) {
        printf("\n[ALERTA] Ciclo detectado (%d/%d).\n", count, g->V);
        return;
    }

    printf("\n>> ORDEN TOPOLOGICO: ");
    for (int i = 0; i < count; i++) printf("%d ", top_order[i]);
    printf("\n");
}

/**
 * @brief Realiza un recorrido en profundidad para ordenar vértices por tiempo de finalización.
 *
 * Esta función es parte del algoritmo de Kosaraju. Visita todos los vértices
 * alcanzables desde v y, después de explorar completamente cada rama, apila el
 * vértice en un stack. El orden inverso de finalización se usa luego para
 * procesar el grafo transpuesto.
 *
 * @param g      [in,out] Grafo original.
 * @param v      [in]     Vértice actual.
 * @param visited [in,out] Arreglo de vértices ya visitados.
 * @param stack   [out]    Pila donde se apilan los vértices.
 * @param top     [in,out] Índice superior de la pila.
 */
void dfsFillOrder(Graph* g, int v, bool visited[], int stack[], int* top) {
    visited[v] = true;
    for (int i = 0; i < g->V; i++)
        if (g->adj[v][i] && !visited[i])
            dfsFillOrder(g, i, visited, stack, top);
    stack[(*top)++] = v;
}

/**
 * @brief Crea el grafo transpuesto de uno dado.
 *
 * El grafo transpuesto invierte el sentido de todas las aristas: si en el
 * grafo original hay una arista de i a j, en el transpuesto habrá una de j a i.
 * Kosaraju usa el transpuesto para encontrar componentes fuertemente conexas.
 *
 * @param g [in] Grafo original.
 * @return Puntero al grafo transpuesto.
 */
Graph* transposeGraph(Graph* g) {
    Graph* g_T = createGraph(g->V);
    for (int i = 0; i < g->V; i++)
        for (int j = 0; j < g->V; j++)
            if (g->adj[i][j]) g_T->adj[j][i] = 1;
    return g_T;
}

/**
 * @brief Recorrido en profundidad para encontrar una componente fuertemente conexa.
 *
 * Recorre el grafo transpuesto desde un vértice y acumula todos los vértices
 * alcanzables en el arreglo component. Estos vértices forman una componente
 * fuertemente conexa del grafo original.
 *
 * @param g_T       [in]     Grafo transpuesto.
 * @param v         [in]     Vértice de inicio.
 * @param visited   [in,out] Arreglo de vértices ya visitados.
 * @param component [out]    Arreglo donde se acumulan los vértices.
 * @param cs        [in,out] Tamaño actual de la componente.
 */
void dfsSCC(Graph* g_T, int v, bool visited[], int component[], int* cs) {
    visited[v] = true;
    component[(*cs)++] = v;
    for (int i = 0; i < g_T->V; i++)
        if (g_T->adj[v][i] && !visited[i])
            dfsSCC(g_T, i, visited, component, cs);
}

/**
 * @brief Encuentra las componentes fuertemente conexas usando el algoritmo de Kosaraju.
 *
 * El algoritmo consta de tres pasos:
 * 1. Realizar un DFS sobre el grafo original y apilar los vértices según su
 *    tiempo de finalización decreciente.
 * 2. Construir el grafo transpuesto.
 * 3. Desapilar los vértices y, para cada vértice no visitado, realizar un DFS
 *    en el grafo transpuesto. Cada DFS descubre una componente fuertemente
 *    conexa.
 *
 * @param g [in,out] Grafo sobre el que se buscan las componentes.
 */
void kosarajuSCC(Graph* g) {
    bool visited[MAX_VERTICES] = {false};
    int stack[MAX_VERTICES];
    int top = 0;

    // Paso 1: llenar el stack con el orden de finalización.
    for (int i = 0; i < g->V; i++)
        if (!visited[i])
            dfsFillOrder(g, i, visited, stack, &top);

    // Paso 2: crear el grafo transpuesto.
    Graph* g_T = transposeGraph(g);

    // Paso 3: reiniciar visitados y procesar en orden inverso de finalización.
    for (int i = 0; i < g->V; i++) visited[i] = false;
    printf("\n[KOSARAJU] Componentes Fuertemente Conexas:\n");
    int scc_count = 0;
    while (top > 0) {
        int v = stack[--top];
        if (!visited[v]) {
            int component[MAX_VERTICES];
            int comp_size = 0;
            dfsSCC(g_T, v, visited, component, &comp_size);
            scc_count++;
            printf("  -> Componente #%d: { ", scc_count);
            for (int i = 0; i < comp_size; i++)
                printf("%d ", component[i]);
            printf("}\n");
        }
    }
    free(g_T);
}

/**
 * @brief Punto de entrada del laboratorio de grafos.
 *
 * Construye dos grafos de ejemplo, demuestra el orden topológico con y sin
 * ciclo, y muestra las componentes fuertemente conexas del segundo grafo.
 *
 * @return 0 al finalizar.
 */
int main()
{
    printf("=== LABORATORIO DE GRAFOS DIRIGIDOS - Albert Gonzalez ===\n");

    printf("=== Ejercicio 1 ===\n");
    Graph* g_academ = createGraph(6);

    // Prelaciones académicas: cada arista indica que una materia es prerrequisito de otra.
    addEdge(g_academ, 0, 2); // prelacion y la materia en si
    addEdge(g_academ, 1, 3);
    addEdge(g_academ, 2, 3);
    addEdge(g_academ, 3, 4);
    addEdge(g_academ, 3, 5);
    addEdge(g_academ, 4, 5);

    printf("=== Orden Topologico Normal ===\n");
    kahnTopologicalSort(g_academ);

    printf("\n=== Orden Topologico FALLIDO ===\n");
    // Se agrega una arista absurda que crea un ciclo, rompiendo el orden topologico.
    addEdge(g_academ, 5, 1);

    // Cuando hay un ciclo, llega un momento en que el algoritmo se queda sin
    // nodos con grado de entrada 0. Por eso el contador de nodos procesados
    // (count) es menor al total de nodos (g->V), lo que dispara la alerta de
    // "Ciclo detectado" y hace fallar el orden lineal por una dependencia circular.
    //
    // En la vida real, esto representaria una paradoja en la que el estudiante
    // no podria cursar la carrera: para ver discretas necesitaria haber visto el
    // proyecto de software final, pero para ver este se requieren varias materias
    // que a su vez son preladas por discretas, generando un bucle interminable.
    kahnTopologicalSort(g_academ);

    printf("\n=== Ejercicio 2 ===\n");
    Graph* g_mapa = createGraph(7);
    addEdge(g_mapa, 0, 1);
    addEdge(g_mapa, 1, 2);
    addEdge(g_mapa, 1, 3);
    addEdge(g_mapa, 2, 0);
    addEdge(g_mapa, 3, 4);
    addEdge(g_mapa, 4, 5);
    addEdge(g_mapa, 5, 3);
    addEdge(g_mapa, 5, 6);

    kosarajuSCC(g_mapa);
    // Hay dos componentes fuertemente conexas de mas de un nodo: {0, 1, 2} y
    // {3, 4, 5}. Esto se debe a que dentro de cada grupo los nodos forman un
    // ciclo cerrado, de modo que el vehiculo queda atrapado en esa zona.
    //
    // Si un conductor estuviese en la interseccion 4, seria imposible llegar a
    // la 0 legalmente. Como estamos en un grafo dirigido (aristas unidireccionales),
    // al avanzar desde la 4 ira a la 5, donde solo hay dos caminos: la 6, donde
    // acaba la carretera, o la 3, y de ahi nuevamente a la 4, formando un ciclo
    // interminable. En cualquiera de los dos casos no podria llegar a la
    // interseccion 0 deseada de manera legitima.

    free(g_academ);
    free(g_mapa);
    return 0;
}
