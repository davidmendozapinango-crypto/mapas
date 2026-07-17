The technology stack for Mapas is intentionally minimal. It is designed to satisfy the academic project requirements while keeping the toolchain simple, portable, and easy to evaluate. The project relies on the C programming language for all logic, standard file I/O for input, and plain text files for configuration.

# **Tech Stack: Mapas - Optimal Route Calculator**

## **Why C?**

*   **Mandatory Requirement:** The project specification and course context require implementation in C, making it the primary and only programming language for the application.
*   **Direct Control Over Data Structures:** C provides fine-grained control over memory layout, which is ideal for implementing efficient graph representations, priority queues, and path lists.
*   **Suitable for Graph Algorithms:** Dijkstra's algorithm and Yen's algorithm can be implemented cleanly in C with arrays, linked lists, and custom structs, without the overhead of higher-level abstractions.
*   **No External Runtime Dependencies:** A C program compiled with GCC produces a standalone executable, simplifying distribution, grading, and execution on target systems.

## **Responsibilities**

*   **User Interface Layer (C Standard I/O):**
    *   **Prompt Management:** Display prompts for the route file, optional weather file, origin place, and destination place.
    *   **Result Display:** Render the best and second-best routes for each transport mode in console tables.
    *   **Interactive Loop:** Manage the post-calculation menu that lets the user change files, change origin/destination, or exit.
    *   **Language:** The MVP uses English prompts and output messages. Spanish support may be added as a post-MVP enhancement.
*   **Core Logic Layer (C11):**
    *   **File Parsing:** Read and validate route and weather configuration files.
    *   **Graph Construction:** Build a directed weighted graph from parsed routes with independent weights for walking, bus, and car.
    *   **Shortest Path Calculation:** Implement Dijkstra's algorithm to find the optimal route per transport mode.
    *   **Alternative Path Calculation:** Implement Yen's algorithm to find the second-best route per transport mode.
    *   **Weather Engine:** Apply per-route weather multipliers and mark routes as impassable when the multiplier is zero.
*   **Data Layer (Plain Text Files):**
    *   **Route Files:** Store places, directed routes, and base costs for walking, bus, and car.
    *   **Weather Files:** Store optional per-route multipliers that modify route costs.
    *   **Example Data:** Provide sample input files for development, testing, and demonstration.

## **MVP-First Implementation Approach**

1.  **Phase 1: Project Scaffolding and Parsing:** Create the source folder structure, implement the route file parser, and implement the optional weather file parser. Validate inputs and report errors clearly.
2.  **Phase 2: Graph Construction and Dijkstra:** Build the directed graph from parsed data. Implement Dijkstra's algorithm with a min-heap priority queue to compute the optimal route for each transport mode.
3.  **Phase 3: Yen's Algorithm and Weather Integration:** Implement Yen's k-shortest paths algorithm to produce second-best routes. Integrate weather multipliers and impassable route handling.
4.  **Phase 4: Console Output, Interactive Loop, and Testing:** Format results as console tables, implement the re-run loop, and add unit tests for parsers, graph operations, and algorithms.

## **Local Development**

*   **Modular Decomposition:** The project is divided into focused modules: `parser`, `graph`, `dijkstra`, `yen`, `weather`, `output`, and `cli`. Each module has a corresponding header file in `include/` and implementation file in `src/`.
*   **Code Style:** The code follows C11 standards, procedural programming style, `snake_case` naming for functions and variables, and `PascalCase` for struct types. Memory allocated dynamically must be freed explicitly.
*   **Asset Management:** Static input data and example files reside in the `data/` directory. Test cases and fixtures are stored in `tests/fixtures/`.
*   **Testing:** Unit tests are written using the Unity testing framework. Each core module has a dedicated test file. Tests are compiled and executed through the Makefile.

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
│   ├── test_parser.c
│   ├── test_graph.c
│   ├── test_dijkstra.c
│   ├── test_yen.c
│   ├── test_weather.c
│   └── fixtures/
│       ├── valid_routes.txt
│       ├── valid_weather.txt
│       └── invalid_routes.txt
├── docs/
│   ├── 01 ProjectGoals.md
│   ├── 02 AppFeatures.md
│   ├── 03 TechStack.md
│   └── ProjectInitial.md
├── README.md
├── Makefile
├── build.bat
└── .gitignore
```

## **Future Enhancements**

*   **CMake Support:** Add a `CMakeLists.txt` for cross-platform builds on Windows, Linux, and macOS.
*   **ncurses Console UI:** Replace plain text prompts with an ncurses-based interface for richer navigation and table rendering.
*   **JSON Input Format:** Support JSON route and weather files as an alternative to the custom text format.
*   **Binary Data Export:** Allow saving computed routes to a binary or structured text file for reporting.

## **Summary**

The selected stack—C11, GCC, plain text files, and Unity for testing—fulfills all project requirements while remaining lightweight and easy to maintain. It enables direct implementation of the required graph algorithms, keeps the build process simple through a Makefile and optional `build.bat`, and supports clean modular development. This stack also provides a clear path for future enhancements such as a richer console UI or cross-platform CMake builds.
