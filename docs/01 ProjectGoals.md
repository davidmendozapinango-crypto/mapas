# Project Title: Mapas - Optimal Route Calculator

## Description

Mapas is a C-based command-line application that computes optimal and second-best travel routes across a city map. The system takes a text file describing places, directed routes, and per-mode travel costs (walking, bus, car). Optionally, it accepts a weather file that modifies route costs through multipliers. The application returns the best and alternative routes between a user-selected origin and destination, considering each available transport mode.

## Objectives

### General Objective

Build a reliable route-planning system in C that reads a city map and optional weather data, then returns the best and second-best routes for each transport mode.

### Specific Objectives

1. Parse route configuration files that define places, directed routes, and per-mode costs.
2. Optionally parse weather configuration files that adjust route costs via multipliers.
3. Compute the shortest path between two points using Dijkstra's algorithm.
4. Compute the second shortest path using Yen's algorithm.
5. Present all computed routes clearly through console-based tables.
6. Support an interactive execution loop that allows users to change inputs and re-run calculations without restarting the program.

## Scope

1. Directed graph model representing city routes and places.
2. Three transport modes: walking (P), bus (B), and car (C).
3. Optional weather impact on route cost and passability.
4. Best and second-best route per transport mode, yielding up to six total results.
5. Interactive console workflow for file loading and route queries.
6. Input validation and meaningful error reporting.
7. Final deliverables include source code, a written report (maximum 5 pages), and demonstrated results.

## Key Functional Components

1. **Input Parser:** Reads and validates route files and optional weather files. Responsible for tokenizing lines, checking syntax, and converting values into internal data structures.
2. **Graph Builder:** Constructs a directed weighted graph from the parsed route data. Each route becomes an edge with separate weights for walking, bus, and car.
3. **Weather Engine:** Applies weather multipliers to edge weights when a weather file is provided. A multiplier of zero marks the route as impassable for the corresponding mode.
4. **Path Finder (Dijkstra):** Computes the optimal route between an origin and a destination for a selected transport mode.
5. **Alternative Path Finder (Yen):** Computes the second-best route for a selected transport mode by generating candidate deviations from the optimal path.
6. **Output Formatter:** Renders routes, total costs, and intermediate stops in a readable console table format.

## Delivery Approach

The delivery is divided into **2 parts**:

*   **Part One (60%):** Core graph parsing, Dijkstra implementation, console output, and basic input handling.
*   **Part Two (40%):** Weather integration, Yen's algorithm for second-best routes, interactive re-run loop, QA validation, final report, and documentation.

## What "MVP Working" Means

A functional MVP means the software completes all development stages—analysis, design, construction, testing, and execution—and demonstrates that it provides a correct response to the situations posed in the project. Specifically, the application reads valid route and optional weather files, computes correct optimal and second-best routes for each transport mode, and displays them in a clean console format.

## Extended-MVP (Next Phase)

The next phase may include support for bidirectional routes, export of results to a text file, ASCII graph visualization, and a multi-language user interface.

### Local Development Checklist

Before testing the MVP, verify:

#### Input Parsing
- [ ] Route file syntax is validated correctly.
- [ ] Weather file syntax is validated correctly.
- [ ] Missing or invalid files are handled with clear error messages.

#### Graph and Algorithms
- [ ] The directed graph is built accurately from parsed routes.
- [ ] Dijkstra's algorithm returns the correct shortest path for each transport mode.
- [ ] Yen's algorithm returns the correct second shortest path for each transport mode.

#### Weather Integration
- [ ] Weather multipliers are applied correctly to route costs.
- [ ] Routes with a zero multiplier are treated as impassable for the affected mode.
- [ ] When no weather file is provided, all routes use their base costs.

#### User Interaction
- [ ] The application prompts for the route file, optional weather file, origin, and destination.
- [ ] Up to six routes are displayed in a clear table format.
- [ ] Users can perform repeated queries without restarting the application.

#### Final Submission Requirements
- [ ] Source code compiles successfully on Windows with GCC.
- [ ] All QA tests pass.
- [ ] Design and results report is completed (maximum 5 pages).
- [ ] README includes clear build and run instructions.

## Future Enhancements (Post-MVP)

*   Support for bidirectional or undirected routes.
*   Export of calculated routes to a text file.
*   ASCII visualization of the city graph and selected routes.
*   Multi-language user interface.

## Technology Selection Note

The mandatory technological requirements include:

*   **Programming Language:** C (C11 recommended).
*   **Storage:** Plain text files for input; in-memory graph and results.
*   **Other Requirements:** Windows-compatible console user interface; no external graphical libraries required.

## How This Document Fits With the Others

This document defines the high-level goals, scope, acceptance criteria, and deliverables of the project. Subsequent documentation will cover the technical design (data structures, graph representation, algorithm implementation), the detailed implementation plan, the test strategy, and the final project report.
