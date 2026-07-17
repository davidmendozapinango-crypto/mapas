#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_VERTICES 20

typedef struct Graph {
    int V;
    int adj[MAX_VERTICES][MAX_VERTICES];
} Graph;

Graph* createGraph(int vertices) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->V = vertices;
    for (int i = 0; i < vertices; i++)
    for (int j = 0; j < vertices; j++)
    g->adj[i][j] = 0;
    return g;
}

void addEdge(Graph* g, int u, int v) {
    if (u < g->V && v < g->V)
    g->adj[u][v] = 1;
}

// --- RETO 1: ALGORITMO DE KAHN (ORDEN TOPOLÓGICO) ---
void kahnTopologicalSort(Graph* g) {
    int in_degree[MAX_VERTICES] = {0};
    for (int i = 0; i < g->V; i++)
        for (int j = 0; j < g->V; j++)
            if (g->adj[i][j]) in_degree[j]++;

    int queue[MAX_VERTICES];
    int front = 0, rear = 0;
    for (int i = 0; i < g->V; i++)
        if (in_degree[i] == 0) queue[rear++] = i;
    int top_order[MAX_VERTICES];
    int count = 0;
    printf("\n[KAHN] Iniciando procesamiento...\n");
    while (front < rear) {
        int u = queue[front++];
        top_order[count++] = u;
        printf("  -> Nodo [%d] liberado\n", u);
        for (int v = 0; v < g->V; v++) {
            if (g->adj[u][v]) {
                in_degree[v]--;
                if (in_degree[v] == 0) queue[rear++] = v;
            }
        }
    }

    if (count != g->V) {
    printf("\n[ALERTA] Ciclo detectado (%d/%d).\n",
    count, g->V);
    return;
    }
    printf("\n>> ORDEN TOPOLÓGICO: ");
    for (int i = 0; i < count; i++) printf("%d ", top_order[i]); printf("\n");
}

// --- RETO 2: ALGORITMO DE KOSARAJU (CFC) ---
void dfsFillOrder(Graph* g, int v, bool visited[], int stack[], int* top) { 
    visited[v] = true;
    for (int i = 0; i < g->V; i++)
        if (g->adj[v][i] && !visited[i])
            dfsFillOrder(g, i, visited, stack, top); stack[(*top)++] = v;
}

Graph* transposeGraph(Graph* g) {
    Graph* g_T = createGraph(g->V);
    for (int i = 0; i < g->V; i++)
        for (int j = 0; j < g->V; j++)
            if (g->adj[i][j]) g_T->adj[j][i] = 1;
    return g_T;
}

void dfsSCC(Graph* g_T, int v, bool visited[], int component[], int* cs) { 
    visited[v] = true;
    component[(*cs)++] = v;
    for (int i = 0; i < g_T->V; i++)
        if (g_T->adj[v][i] && !visited[i])
            dfsSCC(g_T, i, visited, component, cs);
}

void kosarajuSCC(Graph* g) { 
    bool visited[MAX_VERTICES] = {false}; 
    int stack[MAX_VERTICES];
    int top = 0;
    for (int i = 0; i < g->V; i++)
        if (!visited[i])
            dfsFillOrder(g, i, visited, stack, &top);

    Graph* g_T = transposeGraph(g);
    for (int i = 0; i < g->V; i++) visited[i] = false;
    printf("\n[KOSARAJU] Componentes Fuertemente Conexas:\n"); int scc_count = 0;
    while (top > 0) {
        int v = stack[--top];
        if (!visited[v]) {
            int component[MAX_VERTICES];
            int comp_size = 0;
            dfsSCC(g_T, v, visited, component, &comp_size); scc_count++;
            printf("  -> Componente #%d: { ", scc_count);
            for (int i = 0; i < comp_size; i++)
                printf("%d ", component[i]);
            printf("}\n");
        }
    }
    free(g_T);
}

int main() 
{
    printf("=== LABORATORIO DE GRAFOS DIRIGIDOS - Albert Gonzalez ===\n");
    printf("=== Ejercicio 1 ===\n");
    Graph* g_academ = createGraph(6);
    addEdge(g_academ, 0, 2);//prelacion y la materia en si
    addEdge(g_academ, 1, 3);
    addEdge(g_academ, 2, 3);
    addEdge(g_academ, 3, 4);
    addEdge(g_academ, 3, 5);
    addEdge(g_academ, 4, 5);
    printf("=== Orden Topologico Normal ===\n");
    kahnTopologicalSort(g_academ); //-> Para este punto identifique que si funciona correctamnete segun el orden topologico.

    printf("\n=== Orden Topologico FALLIDO ===\n");
    addEdge(g_academ, 5, 1); //-> Pero cuando se le añade la "arista absurda" la consola muestra un ciclo dectectado en el grafo("[ALERTA] Ciclo detectado (2/6)."), lo cual afecta el orden topologico ya que para que un nodo pueda ser "liberado" y puesto en el orden topológico, su grado de entrada debe ser 0, osea, no tiene requisitos pendientes, y si hay un ciclo llegará un momento en que el algoritmo se queda sin nodos con grado 0, por eso, al final, el contador de nodos procesados (count) es menor al total de nodos del grafo (g->V), lo que dispara la alerta de "Ciclo detectado", haciendo que falle su orden lineal por una dependencia circular.
    
    //En la vida real, esto representaria una paradoja en la que el estudiante no podria cursar la carrera, ya que para ver discretas necesitara 1ro haber visto el proyecto de software final, pero para ver este, previamiente se tuvo que haber visto varias materias que a su vez son preladas por discretas, y asi generando un bucle o paradoja interminable.
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
    //Como tal solo hay 2 componentes fuertemente conexas de mas de un nodo ({0, 1, 2} y {3, 4, 5} como se muestran en la salida), y esto es debido a que estas entre si forman un ciclo cerrado, en donde el vehículo queda atrapado como lo mencionaba en los reportes de los ciudadanos.
    //Si un conductor estuviese en la la intersección 4, seria imposible llegar a la 0(legalmente), esto ya que como esta parte de una componente fuertemente conexa como se menciono antes y como estamos en un grafo dirigido(aristas unidireccionales), al intentar avanzr en via, ira a la 5 donde solo hay 2 caminos posibles, la interseccion 6 donde se acaba toda la carretera o la interseccion 3, y de ahi nuevamnete volveria a la 4 formando un ciclo interminable, en donde en cualquiera de las 2 situaciones no podria llegar a la interseccion 0 deseada de manera legitima.
    free(g_academ);
    free(g_mapa);
    return 0;
}