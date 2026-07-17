#include "dijkstra.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INF_COST 1e300
#define EPS 1e-12

typedef struct {
    double dist;
    int vertex;
} HeapNode;

typedef struct {
    HeapNode *data;
    int size;
    int capacity;
} MinHeap;

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

static void heap_destroy(MinHeap *h)
{
    if (!h) return;
    free(h->data);
    free(h);
}

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

    if (!g || src < 0 || src >= g->place_count || dst < 0 || dst >= g->place_count) {
        return result;
    }

    int n = g->place_count;
    double *dist = malloc(sizeof(double) * (size_t)n);
    int *prev = malloc(sizeof(int) * (size_t)n);
    if (!dist || !prev) {
        free(dist);
        free(prev);
        return result;
    }
    for (int i = 0; i < n; i++) {
        dist[i] = INF_COST;
        prev[i] = -1;
    }
    dist[src] = 0.0;

    MinHeap *heap = heap_create(n * 2 + 4);
    if (!heap) {
        free(dist);
        free(prev);
        return result;
    }
    heap_push(heap, 0.0, src);

    while (heap->size > 0) {
        double d;
        int u;
        if (!heap_pop(heap, &d, &u)) break;
        if (d > dist[u] + EPS) continue;
        if (u == dst) break;

        const EdgeList *list = &g->edges[u];
        for (int i = 0; i < list->count; i++) {
            const Route *r = &list->routes[i];
            int v = r->to;
            if (blocked_nodes && blocked_nodes[v]) continue;
            if (is_blocked_edge(u, v, blocked_edges_from, blocked_edges_to, blocked_edge_count)) {
                continue;
            }
            double w = weather_effective_cost(wm, r, mode);
            if (w >= INF_COST / 2.0) continue;
            if (dist[u] + w < dist[v] - EPS) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                heap_push(heap, dist[v], v);
            }
        }
    }

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

    heap_destroy(heap);
    free(dist);
    free(prev);
    return result;
}

void path_free(Path *p)
{
    if (!p) return;
    free(p->places);
    p->places = NULL;
    p->place_count = 0;
    p->total_cost = INF_COST;
}
