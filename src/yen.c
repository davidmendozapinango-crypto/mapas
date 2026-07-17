#include "yen.h"
#include "dijkstra.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INF_COST 1e300
#define EPS 1e-12

static int paths_equal(const Path *a, const Path *b)
{
    if (a->place_count != b->place_count) return 0;
    for (int i = 0; i < a->place_count; i++) {
        if (a->places[i] != b->places[i]) return 0;
    }
    return 1;
}

static double path_cost(const Graph *g, TransportMode mode, const int *places,
                        int count, const WeatherMap *wm)
{
    double total = 0.0;
    for (int i = 0; i < count - 1; i++) {
        int from = places[i];
        int to = places[i + 1];
        double best = INF_COST;
        const EdgeList *list = &g->edges[from];
        for (int j = 0; j < list->count; j++) {
            const Route *r = &list->routes[j];
            if (r->to == to) {
                double c = weather_effective_cost(wm, r, mode);
                if (c < best) best = c;
            }
        }
        if (best >= INF_COST / 2.0) return INF_COST;
        total += best;
    }
    return total;
}

static int add_candidate(Path **candidates, int *count, int *capacity,
                         const Path *candidate)
{
    for (int i = 0; i < *count; i++) {
        if (paths_equal(&(*candidates)[i], candidate)) {
            return 0;
        }
    }
    if (*count >= *capacity) {
        int new_cap = *capacity == 0 ? 4 : *capacity * 2;
        Path *new_items = realloc(*candidates, sizeof(Path) * (size_t)new_cap);
        if (!new_items) return 0;
        *candidates = new_items;
        *capacity = new_cap;
    }
    Path copy;
    copy.place_count = candidate->place_count;
    copy.places = malloc(sizeof(int) * (size_t)candidate->place_count);
    if (!copy.places) return 0;
    memcpy(copy.places, candidate->places, sizeof(int) * (size_t)candidate->place_count);
    copy.total_cost = candidate->total_cost;
    copy.mode = candidate->mode;
    (*candidates)[*count] = copy;
    (*count)++;
    return 1;
}

int yen_find_paths(const Graph *g, TransportMode mode, int src, int dst,
                   const WeatherMap *wm, PathResult *result)
{
    result->best.places = NULL;
    result->best.place_count = 0;
    result->best.total_cost = 0.0;
    result->best.mode = mode;
    result->second = result->best;
    result->has_best = 0;
    result->has_second = 0;

    Path best = dijkstra_find(g, mode, src, dst, wm, NULL, NULL, NULL, 0);
    if (best.place_count == 0) {
        return 0;
    }
    result->best = best;
    result->has_best = 1;

    Path *candidates = NULL;
    int candidate_count = 0;
    int candidate_capacity = 0;

    Path current = best;
    int k = 2;
    for (int i = 0; i < k - 1; i++) {
        for (int spur_idx = 0; spur_idx < current.place_count - 1; spur_idx++) {
            int spur_node = current.places[spur_idx];

            int *blocked_nodes = calloc((size_t)g->place_count, sizeof(int));
            if (!blocked_nodes) continue;
            for (int j = 0; j < spur_idx; j++) {
                blocked_nodes[current.places[j]] = 1;
            }

            int be_count = 0;
            int be_capacity = 4;
            int *be_from = malloc(sizeof(int) * (size_t)be_capacity);
            int *be_to = malloc(sizeof(int) * (size_t)be_capacity);
            if (!be_from || !be_to) {
                free(blocked_nodes);
                free(be_from);
                free(be_to);
                continue;
            }

            Path results[1] = { current };
            int result_count = 1;
            for (int p = 0; p < result_count; p++) {
                const Path *path = &results[p];
                if (spur_idx >= path->place_count - 1) continue;
                int same = 1;
                for (int j = 0; j <= spur_idx; j++) {
                    if (path->places[j] != current.places[j]) {
                        same = 0;
                        break;
                    }
                }
                if (same) {
                    if (be_count >= be_capacity) {
                        be_capacity *= 2;
                        int *new_from = realloc(be_from, sizeof(int) * (size_t)be_capacity);
                        int *new_to = realloc(be_to, sizeof(int) * (size_t)be_capacity);
                        if (!new_from || !new_to) {
                            free(new_from);
                            free(new_to);
                            break;
                        }
                        be_from = new_from;
                        be_to = new_to;
                    }
                    be_from[be_count] = path->places[spur_idx];
                    be_to[be_count] = path->places[spur_idx + 1];
                    be_count++;
                }
            }

            Path spur = dijkstra_find(g, mode, spur_node, dst, wm,
                                      blocked_nodes, be_from, be_to, be_count);
            if (spur.place_count > 0) {
                Path total;
                total.place_count = spur_idx + spur.place_count;
                total.places = malloc(sizeof(int) * (size_t)total.place_count);
                if (total.places) {
                    memcpy(total.places, current.places, sizeof(int) * (size_t)(spur_idx + 1));
                    memcpy(total.places + spur_idx + 1, spur.places + 1,
                           sizeof(int) * (size_t)(spur.place_count - 1));
                    total.mode = mode;
                    double root_cost = path_cost(g, mode, current.places, spur_idx + 1, wm);
                    total.total_cost = root_cost + spur.total_cost;
                    add_candidate(&candidates, &candidate_count, &candidate_capacity, &total);
                    free(total.places);
                }
            }
            path_free(&spur);
            free(blocked_nodes);
            free(be_from);
            free(be_to);
        }
    }

    Path second;
    second.places = NULL;
    second.place_count = 0;
    second.total_cost = INF_COST;
    second.mode = mode;

    for (int i = 0; i < candidate_count; i++) {
        if (!paths_equal(&best, &candidates[i]) &&
            candidates[i].total_cost < second.total_cost - EPS) {
            path_free(&second);
            second = candidates[i];
        } else {
            path_free(&candidates[i]);
        }
    }
    free(candidates);

    if (second.place_count > 0) {
        result->second = second;
        result->has_second = 1;
    }

    return 1;
}
