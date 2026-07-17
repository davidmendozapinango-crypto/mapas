# Research: Route Calculation Core

**Feature**: Route Calculation Core  
**Date**: 2026-07-16

This document records the technical decisions made during the planning phase for the Mapas route-calculation engine.

## Decision: Graph Representation

**Decision**: Use an adjacency list with dynamically allocated arrays of edges.

**Rationale**: An adjacency list is the standard representation for sparse directed graphs. City maps typically have far fewer edges than the maximum possible in a dense graph, so an adjacency list minimizes memory usage while keeping neighbor iteration efficient. Each edge stores the target place index, the route name for weather lookup, and three cost values (walking, bus, car).

**Alternatives considered**:

*   **Adjacency matrix**: Simpler indexing but wastes memory for sparse graphs and makes it harder to store parallel routes between the same pair of places.
*   **Linked lists of edges**: Allows easy insertion but adds pointer-chasing overhead and complicates memory cleanup.

## Decision: Dijkstra Implementation

**Decision**: Implement Dijkstra with a binary min-heap priority queue.

**Rationale**: A binary heap provides `O((V + E) log V)` complexity, which is efficient for the target scale of 50 places and 200 routes. The heap can be implemented with a small fixed-size array because the maximum number of vertices is known and small.

**Alternatives considered**:

*   **Linear search for the minimum distance vertex**: Simpler to code (`O(V^2)`) but slower and does not scale if the map size increases later.
*   **Fibonacci heap**: Better theoretical complexity but significantly more code and memory overhead for a small academic project.

## Decision: Yen's k-Shortest Paths Implementation

**Decision**: Implement Yen's algorithm to find the second shortest path for each transport mode.

**Rationale**: Yen's algorithm generates candidate paths by forcing deviations from the optimal path. It is well suited for finding the second-best path in a directed graph and naturally handles the requirement that the second path must be distinct from the first. With a small graph (<=50 vertices, <=200 edges), the computational cost is acceptable.

**Alternatives considered**:

*   **Suurballe's algorithm**: Also finds disjoint paths but is more complex to implement and does not directly target the second-shortest non-identical path.
*   **Brute-force enumeration**: Impractical even for small graphs and not a viable algorithmic solution.

## Decision: Memory Management Strategy

**Decision**: All dynamically allocated structures are freed explicitly. Modules expose `create` and `destroy` functions for every major structure (graph, path list, heap).

**Rationale**: C does not provide automatic memory management. Centralizing allocation and deallocation in paired functions reduces the risk of leaks and double frees and makes unit testing easier.

**Alternatives considered**:

*   **Arena allocator**: Could simplify cleanup but adds complexity and is unnecessary for a short-lived CLI process.
*   **Static/global buffers**: Avoids allocation errors but limits flexibility and makes testing harder.

## Decision: File Parsing Strategy

**Decision**: Read input files line by line with `fgets`, then parse each line with `sscanf` and manual validation.

**Rationale**: `fgets` plus `sscanf` is portable, requires no external libraries, and is sufficient for the fixed-format input files. Manual validation after parsing allows precise error messages with line numbers.

**Alternatives considered**:

*   **Custom tokenizer**: More flexible but adds code without benefit given the rigid input formats.
*   **Regular expressions with `regex.h`**: Adds POSIX dependency and complexity; `sscanf` is simpler and standard C.

## Decision: Testing Framework

**Decision**: Use the Unity testing framework.

**Rationale**: Unity is a single-file C testing framework designed for embedded and systems projects. It is easy to integrate, has no external dependencies, and supports the test-first workflow required by the constitution.

**Alternatives considered**:

*   **CMocka**: More feature-rich but larger and adds more files to the build.
*   **Custom assert macros**: Simplest but lacks structured test runners and reporting.

## Decision: Weather Application

**Decision**: Apply weather multipliers during graph construction by copying base costs into effective costs. Impassable edges (multiplier or base cost equal to zero) are excluded from the graph for the affected mode.

**Rationale**: Separating base costs from effective costs keeps the graph data structure simple. The same graph can be rebuilt quickly if the user loads a different weather file during the re-run loop.

**Alternatives considered**:

*   **Store multipliers on edges and compute effective costs at query time**: More flexible but requires storing additional state and complicates the graph structure.

## Open Questions Resolved

*   No unresolved clarifications remain. All decisions are aligned with the feature specification and project constitution.
