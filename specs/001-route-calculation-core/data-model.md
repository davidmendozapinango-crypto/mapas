# Data Model: Route Calculation Core

**Feature**: Route Calculation Core  
**Date**: 2026-07-16

This document defines the core data structures used by the Mapas route-calculation engine.

## Entities

### Place

A node in the city graph.

| Field | Type | Description |
|-------|------|-------------|
| `name` | `char*` | Unique alphanumeric-plus-underscore identifier. |
| `index` | `int` | Internal zero-based index assigned when the place is inserted into the graph. |

**Validation rules**:

*   `name` MUST contain only ASCII letters, digits, and underscores.
*   `name` MUST be unique within the route file.

### Route

A directed edge between two places.

| Field | Type | Description |
|-------|------|-------------|
| `name` | `char*` | Unique route identifier. |
| `from` | `int` | Index of the origin place. |
| `to` | `int` | Index of the destination place. |
| `costs[3]` | `double[3]` | Base costs for walking, bus, and car. Index 0 = walking, 1 = bus, 2 = car. |

**Validation rules**:

*   `name` MUST be unique within the route file.
*   `from` and `to` MUST reference known places.
*   Each cost in `costs` MUST be a non-negative decimal number.
*   A cost of zero means the route is impassable for that mode.

### WeatherCondition

A set of multipliers applied to a single route.

| Field | Type | Description |
|-------|------|-------------|
| `route_name` | `char*` | Identifier of the route to which the multipliers apply. |
| `multipliers[3]` | `double[3]` | Multipliers for walking, bus, and car. Index 0 = walking, 1 = bus, 2 = car. |

**Validation rules**:

*   `route_name` MAY reference a route that does not exist; such entries are ignored with a warning.
*   Each multiplier MUST be a non-negative decimal number.
*   A multiplier of zero makes the route impassable for that mode.

### Path

A computed route from origin to destination.

| Field | Type | Description |
|-------|------|-------------|
| `places` | `int*` | Array of place indices representing the sequence from origin to destination. |
| `place_count` | `int` | Number of places in the sequence. |
| `total_cost` | `double` | Sum of effective costs along the path. |
| `mode` | `TransportMode` | Walking, bus, or car. |

**Validation rules**:

*   `places[0]` MUST be the origin.
*   `places[place_count - 1]` MUST be the destination.
*   `total_cost` MUST be the sum of the effective edge costs for the selected mode.

### Graph

The in-memory representation of the city map.

| Field | Type | Description |
|-------|------|-------------|
| `places` | `Place*` | Array of all places. |
| `place_count` | `int` | Number of places. |
| `place_capacity` | `int` | Allocated capacity of the places array. |
| `edges` | `Route**` | Adjacency list. `edges[i]` points to an array of routes leaving place `i`. |
| `edge_counts` | `int*` | Number of outgoing routes for each place. |
| `edge_capacities` | `int*` | Allocated capacity of each outgoing route array. |

**Validation rules**:

*   Every route in the adjacency list MUST reference valid place indices.
*   The graph MUST be rebuilt when the user loads a new route or weather file.

### TransportMode

An enumeration representing the three transport modes.

| Value | Meaning |
|-------|---------|
| `MODE_WALK` | Walking (P) |
| `MODE_BUS` | Bus (B) |
| `MODE_CAR` | Car (C) |

## Relationships

*   A **Graph** contains many **Place** instances.
*   A **Graph** contains many **Route** instances stored in adjacency lists.
*   A **Route** connects exactly two **Place** instances (directed from `from` to `to`).
*   A **WeatherCondition** references exactly one **Route** by name.
*   A **Path** references many **Place** instances and belongs to exactly one **TransportMode**.

## State Transitions

1. **Empty graph**: No places or routes loaded.
2. **Parsed routes**: Places and routes are loaded from the route file.
3. **Weather applied**: Effective costs are computed; impassable edges are excluded per mode.
4. **Query ready**: Origin and destination are validated.
5. **Results computed**: Optimal and second-best paths are stored for each mode.
6. **Displayed**: Results are formatted and printed to the console.
