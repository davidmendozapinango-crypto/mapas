#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

typedef enum {
    MODE_WALK = 0,
    MODE_BUS = 1,
    MODE_CAR = 2,
    MODE_COUNT = 3
} TransportMode;

typedef enum {
    OK = 0,
    ERR_FILE_NOT_FOUND,
    ERR_INVALID_SYNTAX,
    ERR_UNKNOWN_PLACE,
    ERR_NO_ROUTE,
    ERR_MEMORY,
    ERR_INVALID_INPUT
} ErrorCode;

typedef struct {
    char *name;
    int index;
} Place;

typedef struct {
    char *name;
    int from;
    int to;
    double costs[MODE_COUNT];
} Route;

typedef struct {
    int *places;
    int place_count;
    double total_cost;
    TransportMode mode;
} Path;

typedef struct {
    Path best;
    Path second;
    int has_best;
    int has_second;
} PathResult;

#endif
