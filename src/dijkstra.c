/**
 * @file dijkstra.c
 * @brief Implementación del algoritmo de Dijkstra con min-heap.
 *
 * Este archivo calcula el camino de menor costo entre dos lugares de un
 * grafo dirigido. Para hacerlo de forma eficiente, utiliza un montículo
 * mínimo (min-heap) como cola de prioridad, de modo que siempre se expande
 * primero el nodo con la menor distancia acumulada conocida.
 */

#include "dijkstra.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/** @brief Valor que representa un costo infinito dentro del algoritmo. */
#define INF_COST 1e300

/** @brief Tolerancia usada para comparar valores de punto flotante. */
#define EPS 1e-12

/**
 * @brief Nodo del min-heap interno.
 *
 * Cada entrada almacena una distancia acumulada y el vértice asociado. El
 * heap se ordena por distancia, de modo que la raíz siempre sea el nodo con
 * menor distancia pendiente.
 */
typedef struct {
    double dist;
    int vertex;
} HeapNode;

/**
 * @brief Estructura del min-heap.
 *
 * Mantiene un arreglo dinámico de nodos, junto con el tamaño actual y la
 * capacidad reservada. El arreglo crece automáticamente cuando es necesario.
 */
typedef struct {
    HeapNode *data;
    int size;
    int capacity;
} MinHeap;

/**
 * @brief Crea un min-heap vacío con una capacidad inicial.
 *
 * Reserva memoria para la estructura y para el arreglo de nodos. Inicializa
 * el tamaño en cero.
 *
 * @param capacity Capacidad inicial del arreglo de nodos.
 * @return Puntero al heap creado, o NULL si falla alguna reserva.
 */
static MinHeap *heap_create(int capacity)
{
    MinHeap *h = malloc(sizeof(MinHeap));
    if (!h) return NULL;

    h->data = malloc(sizeof(HeapNode) * (size_t)capacity);
    if (!h->data) {
        free(h);
        return NULL;
    }

    h->size = 0;
    h->capacity = capacity;
    return h;
}

/**
 * @brief Libera toda la memoria de un min-heap.
 *
 * @param h [in,out] Heap a destruir.
 */
static void heap_destroy(MinHeap *h)
{
    if (!h) return;
    free(h->data);
    free(h);
}

/**
 * @brief Inserta un nuevo nodo en el min-heap.
 *
 * Si el arreglo interno está lleno, duplica su capacidad antes de insertar.
 * Luego coloca el nuevo nodo al final y lo hace subir (operación sift-up)
 * hasta que se restaure la propiedad del heap: cada padre tiene una distancia
 * menor o igual que la de sus hijos.
 *
 * @param h      [in,out] Heap donde se inserta.
 * @param dist   [in]     Distancia acumulada del nodo.
 * @param vertex [in]     Vértice asociado al nodo.
 */
static void heap_push(MinHeap *h, double dist, int vertex)
{
    if (h->size >= h->capacity) {
        int new_cap = h->capacity * 2;
        HeapNode *new_data = realloc(h->data, sizeof(HeapNode) * (size_t)new_cap);
        if (!new_data) return;
        h->data = new_data;
        h->capacity = new_cap;
    }

    int i = h->size++;
    h->data[i].dist = dist;
    h->data[i].vertex = vertex;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent].dist <= h->data[i].dist) break;

        HeapNode tmp = h->data[parent];
        h->data[parent] = h->data[i];
        h->data[i] = tmp;
        i = parent;
    }
}

/**
 * @brief Extrae el nodo con menor distancia del min-heap.
 *
 * Coloca el último nodo en la raíz y lo hace bajar (operación sift-down)
 * hasta restaurar la propiedad del heap. Escribe la distancia y el vértice
 * extraídos en los punteros de salida.
 *
 * @param h          [in,out] Heap del que se extrae.
 * @param out_dist   [out]    Puntero donde escribir la distancia extraída.
 * @param out_vertex [out]    Puntero donde escribir el vértice extraído.
 * @return 1 si se extrajo un nodo, 0 si el heap estaba vacío.
 */
static int heap_pop(MinHeap *h, double *out_dist, int *out_vertex)
{
    if (h->size == 0) return 0;

    *out_dist = h->data[0].dist;
    *out_vertex = h->data[0].vertex;

    h->data[0] = h->data[--h->size];
    int i = 0;

    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < h->size && h->data[left].dist < h->data[smallest].dist) {
            smallest = left;
        }
        if (right < h->size && h->data[right].dist < h->data[smallest].dist) {
            smallest = right;
        }
        if (smallest == i) break;

        HeapNode tmp = h->data[i];
        h->data[i] = h->data[smallest];
        h->data[smallest] = tmp;
        i = smallest;
    }

    return 1;
}

/**
 * @brief Verifica si una arista específica está bloqueada.
 *
 * Recorre los arreglos paralelos blocked_from y blocked_to buscando el par
 * (from, to). Esta función es usada por el algoritmo de Yen para explorar
 * rutas alternativas sin utilizar ciertas aristas.
 *
 * @param from          [in] Origen de la arista a verificar.
 * @param to            [in] Destino de la arista a verificar.
 * @param blocked_from  [in] Arreglo de orígenes bloqueados.
 * @param blocked_to    [in] Arreglo de destinos bloqueados.
 * @param count         [in] Cantidad de aristas bloqueadas.
 * @return 1 si la arista está bloqueada, 0 en caso contrario.
 */
static int is_blocked_edge(int from, int to, const int *blocked_from,
                           const int *blocked_to, int count)
{
    for (int i = 0; i < count; i++) {
        if (blocked_from[i] == from && blocked_to[i] == to) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Calcula el camino de costo mínimo entre dos lugares.
 *
 * Implementación del algoritmo de Dijkstra. La idea teórica es la siguiente:
 * partir del nodo origen con distancia cero y mantener, para cada nodo, la
 * menor distancia conocida desde el origen. En cada paso se extrae el nodo
 * no procesado con menor distancia (usando el min-heap) y se actualizan las
 * distancias de sus vecinos. Si por un vecino se encuentra un camino más
 * corto, se registra la nueva distancia y se encola el vecino.
 *
 * El algoritmo garantiza el resultado óptimo porque todos los costos son no
 * negativos; una vez que un nodo es extraído del heap, su distancia mínima ya
 * no puede mejorar.
 *
 * @param g                  [in] Grafo sobre el que se busca.
 * @param mode               [in] Modo de transporte a utilizar.
 * @param src                [in] Índice del lugar de origen.
 * @param dst                [in] Índice del lugar de destino.
 * @param wm                 [in] Mapa climático (puede ser NULL).
 * @param blocked_nodes      [in] Arreglo de nodos bloqueados (puede ser NULL).
 * @param blocked_edges_from [in] Orígenes de aristas bloqueadas (puede ser NULL).
 * @param blocked_edges_to   [in] Destinos de aristas bloqueadas (puede ser NULL).
 * @param blocked_edge_count [in] Cantidad de aristas bloqueadas.
 * @return Estructura Path con la ruta encontrada. Si no hay ruta, place_count
 *         será 0 y total_cost será INF_COST.
 */
Path dijkstra_find(const Graph *g, TransportMode mode, int src, int dst,
                   const WeatherMap *wm, const int *blocked_nodes,
                   const int *blocked_edges_from, const int *blocked_edges_to,
                   int blocked_edge_count)
{
    Path result;
    result.places = NULL;
    result.place_count = 0;
    result.total_cost = INF_COST;
    result.mode = mode;

    // Verificar que el grafo y los índices de origen y destino sean válidos.
    // Sin esta comprobación podríamos acceder a memoria fuera de los arreglos.
    if (!g || src < 0 || src >= g->place_count || dst < 0 || dst >= g->place_count) {
        return result;
    }

    int n = g->place_count;

    // dist[i] guardará la menor distancia conocida desde src hasta el nodo i.
    // prev[i] guardará el nodo anterior en el camino más corto hacia i.
    double *dist = malloc(sizeof(double) * (size_t)n);
    int *prev = malloc(sizeof(int) * (size_t)n);
    if (!dist || !prev) {
        free(dist);
        free(prev);
        return result;
    }

    // Inicialmente, todos los nodos están "inaccesibles" (distancia infinita).
    // El nodo origen tiene distancia 0 porque ya nos encontramos en él.
    for (int i = 0; i < n; i++) {
        dist[i] = INF_COST;
        prev[i] = -1;
    }
    dist[src] = 0.0;

    // Creamos la cola de prioridad (min-heap) e insertamos el origen.
    MinHeap *heap = heap_create(n * 2 + 4);
    if (!heap) {
        free(dist);
        free(prev);
        return result;
    }
    heap_push(heap, 0.0, src);

    // Bucle principal del algoritmo de Dijkstra.
    while (heap->size > 0) {
        double d;
        int u;

        // Extraemos el nodo con menor distancia pendiente.
        if (!heap_pop(heap, &d, &u)) break;

        // Si la distancia extraída es mayor que la mejor distancia registrada,
        // significa que este nodo ya fue procesado con un valor mejor y la
        // entrada actual es obsoleta. La ignoramos.
        if (d > dist[u] + EPS) continue;

        // Si llegamos al destino, podemos detenernos anticipadamente porque
        // ya tenemos la distancia mínima definitiva.
        if (u == dst) break;

        // Revisamos cada ruta que sale del nodo u.
        const EdgeList *list = &g->edges[u];
        for (int i = 0; i < list->count; i++) {
            const Route *r = &list->routes[i];
            int v = r->to;

            // Saltamos nodos bloqueados y aristas bloqueadas. Esto permite al
            // algoritmo de Yen forzar caminos alternativos.
            if (blocked_nodes && blocked_nodes[v]) continue;
            if (is_blocked_edge(u, v, blocked_edges_from, blocked_edges_to, blocked_edge_count)) {
                continue;
            }

            // Calculamos el costo efectivo considerando el clima y el modo.
            // Si el resultado es cercano a infinito, la arista es intransitable.
            double w = weather_effective_cost(wm, r, mode);
            if (w >= INF_COST / 2.0) continue;

            // Si encontramos un camino más corto hacia v, actualizamos su
            // distancia, registramos el predecesor y lo encolamos para seguir
            // expandiendo desde él.
            if (dist[u] + w < dist[v] - EPS) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                heap_push(heap, dist[v], v);
            }
        }
    }

    // Si el destino fue alcanzado, reconstruimos la ruta caminando hacia atrás
    // desde dst usando el arreglo de predecesores. Los nodos se guardan en
    // orden inverso para que la ruta quede de src a dst.
    if (dist[dst] < INF_COST / 2.0 && prev[dst] >= 0) {
        int count = 1;
        int v = dst;
        while (v != src) {
            v = prev[v];
            if (v < 0) {
                count = 0;
                break;
            }
            count++;
        }

        if (count > 0) {
            result.places = malloc(sizeof(int) * (size_t)count);
            if (result.places) {
                result.place_count = count;
                v = dst;
                for (int i = count - 1; i >= 0; i--) {
                    result.places[i] = v;
                    v = prev[v];
                }
                result.total_cost = dist[dst];
            }
        }
    }

    // Liberamos toda la memoria auxiliar antes de devolver el resultado.
    heap_destroy(heap);
    free(dist);
    free(prev);
    return result;
}

/**
 * @brief Libera la memoria interna de una estructura Path.
 *
 * Libera el arreglo de lugares y restablece los campos a valores seguros.
 * Después de llamar a esta función, el Path ya no debe usarse hasta volver a
 * ser inicializado.
 *
 * @param p [in,out] Camino cuya memoria se desea liberar.
 */
void path_free(Path *p)
{
    if (!p) return;
    free(p->places);
    p->places = NULL;
    p->place_count = 0;
    p->total_cost = INF_COST;
}
