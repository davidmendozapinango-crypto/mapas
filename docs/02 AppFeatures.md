# App Features

This document describes the functional features of **Mapas**, a C-based command-line application that calculates optimal and second-best travel routes across a city map. The application considers three transport modes—walking, bus, and car—and optionally applies weather conditions that modify route costs and passability.

## MVP Scope (Proof-of-Concept Version)

The following is a comprehensive list of features for the **Mapas** application, categorized by functional area:

### **1. File Input and Validation**

*   **[F1.1] Load Route File:** Prompt the user for the path to a route configuration file. Parse the file to extract place names, directed routes, and the associated costs for walking, bus, and car.
*   **[F1.2] Load Weather File (Optional):** Prompt the user for an optional weather configuration file. Parse per-route multipliers that affect walking, bus, and car costs.
*   **[F1.3] Input Validation:** Detect and report malformed lines, invalid numeric values, duplicate route names, references to undefined places, and missing or inaccessible files. The application must not crash on invalid input.

### **2. Graph Modeling**

*   **[F2.1] Build Directed Graph:** Convert parsed route data into an internal directed weighted graph. Each place becomes a node, and each route becomes a directed edge with three independent weights.
*   **[F2.2] Place Management:** Store place names as graph nodes and support efficient lookup by name to validate origins, destinations, and route endpoints.
*   **[F2.3] Route Direction Handling:** Respect the directional nature of each route as defined in the input file. A route from A to B does not imply a route from B to A unless explicitly declared.

### **3. Route Calculation**

*   **[F3.1] Optimal Route with Dijkstra:** Compute the lowest-cost route between a selected origin and destination for each transport mode using Dijkstra's algorithm.
*   **[F3.2] Second-Best Route with Yen:** Compute the second-best route for each transport mode using Yen's k-shortest paths algorithm, producing a meaningfully different path from the optimal one.
*   **[F3.3] Mode-Specific Results:** Generate up to six route results per query: the best and second-best routes for walking, bus, and car, when such routes exist.

### **4. Weather Impact**

*   **[F4.1] Apply Weather Multipliers:** Adjust the cost of each route by multiplying its base cost by the weather coefficient provided for the corresponding transport mode.
*   **[F4.2] Impassable Routes:** Treat any route with a zero weather multiplier as unavailable for the affected transport mode, removing it from the graph for that mode.
*   **[F4.3] Neutral Weather Default:** When no weather file is provided, all routes retain their base costs and remain passable for every mode.

### **5. User Interaction and Output**

*   **[F5.1] Interactive Query Flow:** Prompt the user for the route file, optional weather file, origin place, and destination place. Validate that origin and destination exist in the loaded graph.
*   **[F5.2] Console Table Output:** Display computed routes in a clear console table format, showing the sequence of places, total cost, and transport mode for each result.
*   **[F5.3] Re-run Loop:** After displaying results, ask the user whether to load different files or compute a new route with the same files. The program continues running until the user chooses to exit.

## MVP Behavior

The **Minimum Viable Product (MVP)** is defined by the successful implementation of the core functional areas outlined above. Its behavior includes:

*   **[B1] Successful File Loading:** The application reads a valid route file and, if provided, a valid weather file. It builds the graph and reports any parsing errors without terminating unexpectedly.
*   **[B2] Correct Optimal Routes:** For a given origin and destination, Dijkstra's algorithm returns the correct lowest-cost route for each transport mode, considering both base costs and weather adjustments.
*   **[B3] Correct Second-Best Routes:** Yen's algorithm returns a valid second-best route for each mode, ensuring it is distinct from the optimal route and respects impassable edges.
*   **[B4] Up to Six Results:** The application displays at most six routes: the best and second-best for walking, bus, and car. If no valid route exists for a mode, the output clearly indicates that the route is unavailable.
*   **[B5] Continuous Interaction:** After each calculation, the user can choose to modify the route file, weather file, origin, destination, or exit the application.

## Extended-MVP Features

These features build upon the core requirements to enhance the user experience and overall impact:

*   **[EF1] Input File Templates:** Provide example route and weather files that demonstrate the expected syntax and structure for users.
*   **[EF2] Summary Statistics:** Display summary information after loading, such as the number of places, routes, and currently passable routes per mode.
*   **[EF3] Error Recovery:** Allow the user to re-enter a file path or query input immediately after a validation failure, without restarting the program.
*   **[EF4] Cost Comparison Table:** Present a side-by-side comparison of total costs for the best and second-best routes across all three modes.

## Post-MVP Features

### **Essential Improvements**

*   **[PI1] Bidirectional Route Support:** Allow the input file to declare routes as bidirectional, reducing duplication for two-way streets.
*   **[PI2] Result Export:** Save computed routes to a text file for later review or inclusion in the project report.
*   **[PI3] History of Queries:** Keep a session history of computed routes so users can recall previous results without recalculating.

### **Additional Capabilities**

*   **[AC1] ASCII Map Visualization:** Render a simple ASCII representation of the city graph, highlighting the computed routes.
*   **[AC2] Batch Processing:** Accept origin-destination pairs from a file and generate all routes in a single execution.
*   **[AC3] Multi-Language UI:** Support Spanish and English prompts and output messages.

### **Practical Notes for Developers**

*   **[PN1] Memory Management:** Since the application is written in C, all dynamically allocated graph structures, path lists, and parsing buffers must be properly freed before exit to avoid memory leaks.
*   **[PN2] Algorithm Choice:** Dijkstra should use a min-heap or priority queue for acceptable performance on larger graphs. Yen's algorithm should reuse Dijkstra computations where possible.
*   **[PN3] Edge Weight Representation:** Use `double` or `float` for costs to support fractional values such as `2.5`. Use a sentinel value such as `INFINITY` to represent impassable edges.

## Additional Features (Longer-Term)

*   **[LF1] Real-Time Weather API Integration:** Allow the application to fetch current weather conditions from an external API instead of a static file.
*   **[LF2] Graphical User Interface:** Build a cross-platform GUI using a lightweight library to visualize the graph and selected routes interactively.
*   **[LF3] Multi-Criteria Optimization:** Extend the route calculation to optimize for additional criteria such as travel time, fuel consumption, or scenic value.
