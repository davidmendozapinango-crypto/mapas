<!--
SYNC IMPACT REPORT
- Version change: 0.0.0 → 1.0.0
- Modified principles: N/A (previous file was unfilled template)
- Added sections: Core Principles (5), Technology Constraints, Development Workflow, Governance
- Removed sections: N/A
- Templates requiring updates:
  - .specify/templates/plan-template.md: ✅ no changes required (generic Constitution Check reference)
  - .specify/templates/spec-template.md: ✅ no changes required (no constitution-specific rules)
  - .specify/templates/tasks-template.md: ✅ no changes required (no constitution-specific task types)
- Runtime guidance docs:
  - README.md: ✅ no constitution references to update
  - docs/01 ProjectGoals.md: ✅ aligned with principles
  - docs/02 AppFeatures.md: ✅ aligned with principles
  - docs/03 TechStack.md: ✅ aligned with principles
- Deferred TODOs: None
-->

# Mapas Constitution

## Core Principles

### I. Simplicity First

Every module, function, and data structure MUST earn its place. Avoid speculative abstractions, unused generality, and dependencies that do not directly support the required route-calculation features. The graph model, parsers, and algorithms must be no more complex than the problem demands.

**Rationale**: The project is an academic CLI tool in C. Extra complexity increases the risk of memory errors, slows compilation, and makes grading and maintenance harder. YAGNI applies.

### II. Maintainability Through Modularity

The codebase MUST be organized into cohesive modules with single responsibilities: parsing, graph representation, Dijkstra search, Yen search, weather application, console output, and CLI flow. Each module exposes a clear public interface through a header file and hides internal details.

**Rationale**: A well-decomposed C project is easier to test, review, and extend. Isolating algorithms from I/O and data structures from formatting reduces coupling and localizes bugs.

### III. Code Quality and Testing Discipline

All core modules MUST have automated unit tests. New logic MUST be accompanied by tests that verify expected behavior and representative edge cases. Code MUST compile without warnings under `-Wall -Wextra -Wpedantic` and must pass static analysis where feasible.

**Rationale**: C lacks memory safety guarantees. Tests and strict compiler warnings catch null dereferences, leaks, and logic errors before they reach users or graders.

### IV. Algorithmic Correctness and Performance

Dijkstra's algorithm MUST produce the true shortest path for each transport mode. Yen's algorithm MUST produce a valid second-shortest path that is distinct from the first. Both algorithms MUST use appropriate data structures (e.g., a min-heap priority queue) to keep runtime acceptable for the expected city-map sizes.

**Rationale**: The project's central value is correct route calculation. Incorrect or unnecessarily slow algorithms defeat the purpose of the application.

### V. Explicit Input/Output Contracts

Route files and weather files MUST follow documented, validated formats. Parser errors MUST produce clear messages that include line numbers and a description of the failure. Console output MUST present routes, modes, and costs in a readable table format.

**Rationale**: The user interacts with the system entirely through text files and console output. Clear contracts and error messages make the tool usable and the grading process transparent.

## Technology Constraints

*   **Programming Language**: C11. No C++ or external runtime environments.
*   **Input/Output**: Plain text files for route and weather data; standard input/output for the interactive console interface.
*   **Build Tools**: Makefile as the primary build system, with an optional `build.bat` for Windows convenience.
*   **Testing**: Unity or an equivalent lightweight C unit-testing framework.
*   **Dependencies**: Only standard C libraries are allowed for the MVP. External libraries may be considered only after the MVP is complete and must be justified.
*   **Platform**: Primary target is Windows with GCC. Linux/WSL compatibility is encouraged but secondary.

## Development Workflow

1.  **Specification Before Code**: Every implementation phase must reference an approved feature or technical specification in `docs/`.
2.  **Test-First for Core Logic**: Write or update unit tests before implementing algorithmic or parser changes. Tests must fail before the implementation is written.
3.  **Small, Focused Commits**: Each commit should address one concern (e.g., a parser fix, a new test, a refactored function).
4.  **Code Review Checklist**: Before considering a change complete, verify it compiles cleanly, passes tests, does not leak memory, and respects the modular boundaries defined in this constitution.
5.  **Documentation in Sync**: Update `README.md`, build scripts, and relevant `docs/` files whenever the interface, file formats, or build process changes.

## Governance

*   **Authority**: This constitution governs all design, implementation, and review decisions for the Mapas project. When a decision conflicts with a principle, the constitution wins unless the constitution itself is formally amended.
*   **Amendments**: Changes to this document require a explicit proposal, a brief rationale, and approval from the project owner. Amendments must update the version number and the last-amended date.
*   **Versioning**: Follow semantic versioning. A MAJOR bump indicates a backward-incompatible governance change or principle removal. A MINOR bump indicates a new principle or materially expanded guidance. A PATCH bump indicates clarifications, wording improvements, or typo fixes.
*   **Compliance Review**: Every implementation plan and feature specification MUST check alignment with this constitution before work begins. Code that violates a MUST principle must be corrected before submission.

**Version**: 1.0.0 | **Ratified**: 2026-07-16 | **Last Amended**: 2026-07-16
