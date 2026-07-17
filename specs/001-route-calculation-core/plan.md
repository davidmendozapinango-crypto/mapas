# Implementation Plan: Route Calculation Core

**Branch**: `001-route-calculation-core` | **Date**: 2026-07-16 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `/specs/001-route-calculation-core/spec.md`

**Note**: This plan covers the initial implementation of the Mapas route-calculation engine: parsing route and weather files, building a directed weighted graph, computing optimal and second-best routes with Dijkstra and Yen, and presenting results through a Spanish-language interactive CLI.

## Summary

Build a C11 CLI application that reads a city map and optional weather data, computes the best and second-best routes for walking, bus, and car, and displays the results in console tables. The implementation is split into modular components (parser, graph, Dijkstra, Yen, weather, output, CLI) and validated with unit and integration tests using Unity.

## Technical Context

**Language/Version**: C11

**Primary Dependencies**: None for the MVP. Only the C standard library is used.

**Storage**: Plain text files for route and weather input; in-memory graph and results.

**Testing**: Unity lightweight C testing framework.

**Target Platform**: Windows with GCC. Linux/WSL compatibility is a secondary goal.

**Project Type**: CLI desktop application.

**Performance Goals**: Parse a 50-place, 200-route file in under 1 second; compute all six routes in under 2 seconds.

**Constraints**: No external libraries for the MVP; compile cleanly with `-Wall -Wextra -Wpedantic`; all dynamic memory must be explicitly freed.

**Scale/Scope**: City maps up to 50 places and 200 routes; single-user interactive console sessions.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Simplicity First | Pass | No speculative abstractions; only standard C libraries and modules required by the feature. |
| II. Maintainability Through Modularity | Pass | Source is split into parser, graph, Dijkstra, Yen, weather, output, and CLI modules. |
| III. Code Quality and Testing Discipline | Pass | Core modules will have Unity unit tests; build uses strict compiler warnings. |
| IV. Algorithmic Correctness and Performance | Pass | Dijkstra uses a binary min-heap; Yen's algorithm produces distinct second-best paths. |
| V. Explicit Input/Output Contracts | Pass | File formats and error messages are documented in the spec and contracts. |

## Project Structure

### Documentation (this feature)

```text
specs/001-route-calculation-core/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
mapas/
├── src/
│   ├── main.c
│   ├── cli.c
│   ├── parser.c
│   ├── graph.c
│   ├── dijkstra.c
│   ├── yen.c
│   ├── weather.c
│   └── output.c
├── include/
│   ├── types.h              # shared enums and structs used across modules
│   ├── cli.h
│   ├── parser.h
│   ├── graph.h
│   ├── dijkstra.h
│   ├── yen.h
│   ├── weather.h
│   └── output.h
├── data/
│   ├── routes_example.txt
│   └── weather_example.txt
├── tests/
│   ├── unity/                 # Unity testing framework source
│   ├── test_parser.c
│   ├── test_graph.c
│   ├── test_dijkstra.c
│   ├── test_yen.c
│   ├── test_weather.c
│   ├── test_output.c
│   ├── test_cli_integration.c
│   ├── test_weather_integration.c
│   ├── test_rerun_integration.c
│   ├── test_cli_loop.c
│   ├── test_cli_errors.c      # FR-010 error-path unit tests
│   └── fixtures/
│       ├── valid_routes.txt
│       ├── valid_weather.txt
│       ├── invalid_routes.txt
│       └── invalid_weather.txt
├── build/                     # generated executables and object files
├── docs/
├── README.md
├── Makefile
├── build.bat
└── .gitignore
```

**Structure Decision**: Single C project with a flat module layout. Each module has a public header in `include/` and implementation in `src/`. `include/types.h` is a cross-cutting shared header for enums and structs that multiple modules need without pulling in a full module interface. `build/` is the generated output directory for executables and object files. Tests mirror the module structure and use fixtures in `tests/fixtures/`. This layout matches the technology constraints in the constitution and keeps the project simple for an academic MVP.
