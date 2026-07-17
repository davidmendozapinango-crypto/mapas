# Feature Specification: Route Calculation Core

**Feature Branch**: `001-route-calculation-core`

**Created**: 2026-07-16

**Status**: Approved

**Input**: User description: "Initial project features. The specification must include: 1) Business objective and operational context of the calculation. 2) Command input contract. 3) Decision rules by calculation paths. 4) Required internal dependencies. 5) Handling of business errors with clear messages for each failed precondition. 6) Verifiable and measurable acceptance criteria. 7) Include the implementation of all necessary tests. 8) Create the branch respective."

## Business Objective and Operational Context

Mapas is a route-planning tool that helps users find the best and second-best travel routes between two places in a city. The system receives a map file describing places, directed routes, and the cost of traveling each route on foot, by bus, or by car. Optionally, it can receive a weather file that modifies those costs. The user then provides an origin and a destination, and the system returns the optimal and second-best routes for each transport mode.

The operational context is an academic CLI application written in C. The user runs the program from a terminal, supplies files interactively, and reads the results as formatted console output. The primary value is correct, fast, and understandable route calculation that adapts to current weather conditions.

## Clarifications

### Session 2026-07-16

*   **Q: What functionality is explicitly out of scope for this MVP?** → **A:** Keep MVP strictly to interactive CLI route calculation (best + second-best per mode, weather optional). No result persistence, no GUI, no bidirectional routes, no import/export.
*   **Q: What language should the CLI prompts and error messages use?** → **A:** Spanish — aligns with the original project description and academic context.
*   **Q: How should the system handle duplicate place or route names in the input files?** → **A:** Ignore duplicates and keep the first occurrence, continuing with the rest of the file.
*   **Q: What is the maximum supported city-map size for the MVP?** → **A:** 50 places and 200 routes.
*   **Q: How should the application behave when a file contains invalid lines: abort immediately or skip invalid lines and continue?** → **A:** Skip invalid lines for the weather file but abort for the route file, because the route file is the primary data source.

## User Scenarios & Testing

### User Story 1 - Load Map and Calculate Optimal and Second-Best Routes (Priority: P1)

A user wants to find the best way to travel from one place to another in the city. They start the program, provide a route file, optionally provide a weather file, enter the origin and destination, and receive the best and second-best routes for walking, bus, and car.

**Why this priority**: This is the core functionality of the entire application. Without it, no other feature delivers value.

**Independent Test**: The application can be launched, a valid route file can be loaded, an origin and destination can be entered, and the program displays the optimal and second-best routes for each available transport mode.

**Acceptance Scenarios**:

1. **Given** a valid route file with at least two connected places, **When** the user enters the route file path, origin, and destination, **Then** the system displays the optimal route, the second-best route, and total cost for each transport mode.
2. **Given** a valid route file and a valid weather file, **When** the user loads both and enters origin and destination, **Then** the displayed costs reflect the weather multipliers.

---

### User Story 2 - Handle Weather Adjustments (Priority: P2)

A user wants to see how current weather affects travel options. They load a weather file that makes some routes impassable or more expensive, and the system recalculates the best routes accordingly.

**Why this priority**: Weather adjustment is a distinguishing feature of the project, but the application remains usable without it.

**Independent Test**: Loading a weather file that sets a walking multiplier to zero causes the system to exclude that route from walking results and still produce valid bus and car results.

**Acceptance Scenarios**:

1. **Given** a route with a base walking cost greater than zero, **When** the weather file sets its walking multiplier to zero, **Then** the route is not included in walking route calculations.
2. **Given** a route with a base car cost, **When** the weather file sets its car multiplier to 1.5, **Then** the displayed car cost equals the base cost multiplied by 1.5.

---

### User Story 3 - Continuously Query Multiple Routes (Priority: P3)

A user wants to compute several different routes during the same session without restarting the program. After the first calculation, they choose to load different files or query a new origin-destination pair.

**Why this priority**: This improves usability and matches the project requirement to continue execution after the first calculation.

**Independent Test**: After the first route is displayed, the user can select the option to compute a new route and the program prompts for new inputs without terminating.

**Acceptance Scenarios**:

1. **Given** a completed calculation, **When** the user chooses to query a new route with the same files, **Then** the system prompts for a new origin and destination and displays new results.
2. **Given** a completed calculation, **When** the user chooses to load different files, **Then** the system prompts for new file paths and rebuilds the graph before the next query.

---

### Edge Cases

*   What happens when the route file is missing or inaccessible? The system must display a clear error and prompt the user again.
*   What happens when a line in the route file has invalid syntax? The system must report the line number and the nature of the error, then abort loading and prompt for a corrected file.
*   What happens when the origin or destination does not exist in the loaded map? The system must report that the place is unknown and prompt for valid input.
*   What happens when no route exists between origin and destination for a given mode? The system must display "No route available" for that mode.
*   What happens when a route has a base cost of zero for a mode? The system must treat that route as impassable for that mode.
*   What happens when the weather file references a route that does not exist? The system must report the unknown route and ignore that entry.
*   What happens when the weather file contains invalid numeric values? The system must report the invalid value and its line number.

## Command Input Contract

The application accepts all inputs through the terminal using interactive prompts.

| Step | Prompt | Input | Validation |
|------|--------|-------|------------|
| 1 | `Ruta del archivo de rutas:` | Absolute or relative path to a route configuration file | File must exist and be readable |
| 2 | `Ruta del archivo de clima (opcional, presione Enter para omitir):` | Absolute or relative path to a weather configuration file, or empty | If provided, file must exist and be readable |
| 3 | `Lugar de origen:` | A place name present in the route file | Name must match a known place exactly |
| 4 | `Lugar de destino:` | A place name present in the route file | Name must match a known place exactly and be different from the origin |
| 5 | `Elija una accion: [n]ueva ruta, [c]ambiar archivos, [s]alir` | Single character `n`, `c`, or `s` | Must be one of the allowed options |

Route file line format:

```
Lugares
nombre_lugar_1
nombre_ruta -> lugar_partida:lugar_llegada = P:costo; B:costo; C:costo
```

Weather file line format:

```
nombre_ruta = P:coeficiente; B:coeficiente; C:coeficiente
```

## Decision Rules by Calculation Paths

*   **Route passability by base cost**: If a route declares a cost of zero for a transport mode, the route is impassable for that mode.
*   **Route passability by weather**: If the weather file declares a multiplier of zero for a transport mode on a route, the route is impassable for that mode.
*   **Weather cost adjustment**: If a route is passable and a weather multiplier is provided, the effective cost equals the base cost multiplied by the weather coefficient.
*   **Neutral weather**: If no weather file is loaded, the effective cost equals the base cost.
*   **Optimal path calculation**: For each transport mode, the system computes the lowest-cost path from origin to destination using the effective costs.
*   **Second-best path calculation**: For each transport mode, the system computes the next lowest-cost path that is not identical to the optimal path.
*   **Maximum results**: The system displays up to six routes: best and second-best for walking, bus, and car.
*   **Missing results**: If a path cannot be computed for a mode, the output clearly states that the route is unavailable for that mode.

## Requirements

### Functional Requirements

*   **FR-001**: The system MUST parse a route file and extract all place names, route names, directed endpoints, and per-mode costs.
*   **FR-002**: The system MUST optionally parse a weather file and extract per-route, per-mode multipliers.
*   **FR-003**: The system MUST validate the syntax of every route-file line. If a route-file line is invalid, the system MUST abort loading, report the line number and expected format, and prompt for a corrected file.
*   **FR-003a**: The system MUST validate the syntax of every weather-file line. If a weather-file line is invalid, the system MUST report the line number, skip the invalid line, and continue loading the remaining valid lines.
*   **FR-003b**: The system MUST treat duplicate place or route names as non-fatal: keep the first occurrence, ignore subsequent duplicates, and display a warning with the line number.
*   **FR-004**: The system MUST build a directed graph from the route file, with separate edge weights for walking, bus, and car.
*   **FR-005**: The system MUST compute the optimal route for each transport mode using Dijkstra's algorithm.
*   **FR-006**: The system MUST compute the second-best route for each transport mode using Yen's algorithm.
*   **FR-007**: The system MUST apply weather multipliers to effective costs before running the algorithms.
*   **FR-008**: The system MUST display all computed routes in a console table showing transport mode, sequence of places, and total cost.
*   **FR-009**: The system MUST support an interactive re-run loop so the user can load new files or query new routes without exiting.
*   **FR-010**: The system MUST handle missing or invalid inputs by printing the specific error message and recovery action defined in the Handling of Business Errors table, then prompt the user to retry the failing input.

### MVP Out of Scope

The following capabilities are explicitly excluded from the MVP and may be considered for future phases:

*   Persisting calculated routes to a file.
*   Graphical or ncurses-based user interface.
*   Bidirectional route declarations.
*   Importing route or weather data from formats other than the specified plain text files.

### Key Entities

*   **Place**: A node in the city graph, identified by a unique alphanumeric-plus-underscore name.
*   **Route**: A directed edge between two places, identified by a unique name and carrying three base costs (walking, bus, car).
*   **WeatherCondition**: A set of three multipliers (walking, bus, car) applied to a single route.
*   **Path**: A sequence of places from origin to destination, with an associated total cost and transport mode.

## Success Criteria

### Measurable Outcomes

*   **SC-001**: The system parses a valid route file at the maximum supported size of 50 places and 200 routes in less than 1 second.
*   **SC-002**: The system computes the optimal and second-best routes for all three modes in less than 2 seconds on the reference test map.
*   **SC-003**: The system displays up to six routes correctly, with each route listing the exact sequence of places and total cost.
*   **SC-004**: All error messages identify the failing precondition (missing file, invalid syntax, unknown place, disconnected route) in plain language.
*   **SC-005**: 100% of the core parser, graph, Dijkstra, and Yen modules are covered by automated unit tests.
*   **SC-006**: A user can complete a full query cycle (load files, enter origin/destination, view results, quit) in under 5 minutes on first use.

## Required Internal Dependencies

This feature depends on the following internal components, which must be implemented before or alongside this feature:

*   Route file parser (`src/parser.c` / `include/parser.h`)
*   Weather file parser (`src/parser.c` / `include/parser.h`)
*   Directed graph data structure (`src/graph.c` / `include/graph.h`)
*   Dijkstra shortest-path implementation (`src/dijkstra.c` / `include/dijkstra.h`)
*   Yen's k-shortest paths implementation (`src/yen.c` / `include/yen.h`)
*   Weather application engine (`src/weather.c` / `include/weather.h`)
*   Console output formatter (`src/output.c` / `include/output.h`)
*   Interactive CLI loop (`src/cli.c` / `include/cli.h`)

## Handling of Business Errors

| Failed Precondition | Error Message Example | Recovery Action |
|---------------------|-----------------------|-----------------|
| Route file not found | `Error: no se pudo abrir el archivo de rutas "maps/ruta.txt".` | Prompt the user to enter the path again. |
| Invalid route file syntax | `Error: sintaxis invalida en la linea 4 del archivo de rutas. Formato esperado: nombre_ruta -> lugar_partida:lugar_llegada = P:costo; B:costo; C:costo` | Abort loading and prompt for a corrected route file. |
| Unknown origin place | `Error: "Casa" no es un lugar conocido en el mapa cargado.` | Prompt the user to enter a valid origin place. |
| Unknown destination place | `Error: "Mercado" no es un lugar conocido en el mapa cargado.` | Prompt the user to enter a valid destination place. |
| No route available for a mode | `No hay ruta disponible en BUS desde Heladeria hasta Ferreteria.` | Display the message in the results table and continue with other modes. |
| Weather file references unknown route | `Advertencia: la linea 2 del clima referencia la ruta desconocida "RutaZ" y sera ignorada.` | Continue processing other weather entries. |
| Weather file contains invalid multiplier | `Advertencia: coeficiente invalido en la linea 3 del archivo de clima. Se esperaba un valor numerico para P, B o C. Linea ignorada.` | Skip the invalid line and continue loading the remaining weather entries. |
| Duplicate place or route name | `Advertencia: nombre duplicado "RutaA" en la linea 5. Se usara la primera ocurrencia.` | Continue loading the file using the first occurrence. |

## Assumptions

*   Input files are plain text encoded in UTF-8 without a byte-order mark.
*   Place names and route names are unique within a single file.
*   All cost and multiplier values are non-negative decimal numbers.
*   The entire graph fits in available memory.
*   The MVP supports city maps of up to 50 places and 200 routes.
*   The user enters file paths and place names interactively through the terminal.
*   The application runs on Windows with GCC, with Linux/WSL compatibility as a secondary goal.

## Test Strategy

The following tests must be implemented to verify this feature:

*   **Unit tests for the parser**: valid route file, invalid route syntax, missing file, duplicate route names.
*   **Unit tests for the graph builder**: directed edges, multiple parallel routes between the same places, correct mode-specific weights.
*   **Unit tests for Dijkstra**: simple path, no path, multiple equal-cost paths, graph with cycles.
*   **Unit tests for Yen's algorithm**: distinct second path, identical second path handling, no second path.
*   **Unit tests for weather application**: multiplier application, zero-multiplier impassability, missing weather file.
*   **Integration tests for the CLI**: full user journey with valid inputs, full user journey with invalid file path, full user journey with weather file.
*   **Acceptance tests for output format**: verify that tables include mode, route sequence, and total cost for up to six routes.

