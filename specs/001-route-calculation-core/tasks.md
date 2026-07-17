---
description: "Task list for Route Calculation Core feature implementation"
---

# Tasks: Route Calculation Core

**Input**: Design documents from `/specs/001-route-calculation-core/`

**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Tests are included because the feature specification explicitly requires automated unit and integration tests for all core modules.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [x] T001 Create project directory structure (`src/`, `include/`, `tests/`, `tests/fixtures/`, `data/`, `build/`) per plan.md
- [x] T002 [P] Add Unity testing framework source files under `tests/unity/`
- [x] T003 [P] Create `Makefile` with strict warning flags (`-Wall -Wextra -Wpedantic`) and `make`, `make test`, `make clean` targets
- [x] T004 [P] Create `build.bat` for Windows without Make
- [x] T005 [P] Create example route file `data/routes_example.txt` matching the route-file contract
- [x] T006 [P] Create example weather file `data/weather_example.txt` matching the weather-file contract
- [x] T007 [P] Create `.gitignore` ignoring `build/`, editor files, and temporary artifacts

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core data structures, parser, and fixtures that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [x] T008 Define shared types and constants (`TransportMode`, `Place`, `Route`, `Path`, error codes) in `include/types.h`
- [x] T009 Implement directed graph data structure (`Graph`, adjacency lists, create/destroy) in `include/graph.h` and `src/graph.c`
- [x] T010 [P] Create valid test fixtures `tests/fixtures/valid_routes.txt` and `tests/fixtures/valid_weather.txt`
- [x] T011 [P] Create invalid test fixtures `tests/fixtures/invalid_routes.txt` and `tests/fixtures/invalid_weather.txt`
- [x] T012 [P] Write unit tests for route and weather parser in `tests/test_parser.c`
- [x] T013 [P] Implement route file parser and weather file parser in `include/parser.h` and `src/parser.c`

**Checkpoint**: Foundation ready - user story implementation can now begin

---

## Phase 3: User Story 1 - Load Map and Calculate Optimal and Second-Best Routes (Priority: P1) 🎯 MVP

**Goal**: Launch the program, load a route file, enter origin and destination, and display the best and second-best routes for walking, bus, and car.

**Independent Test**: Run `./build/mapas`, enter `data/routes_example.txt`, origin `Heladeria`, destination `Parque`. The console table shows optimal and second-best routes for each transport mode.

### Tests for User Story 1 (REQUIRED) ⚠️

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [x] T014 [P] [US1] Write unit tests for graph builder in `tests/test_graph.c`
- [x] T015 [P] [US1] Write unit tests for Dijkstra's algorithm in `tests/test_dijkstra.c`
- [x] T016 [P] [US1] Write unit tests for Yen's k-shortest paths in `tests/test_yen.c`
- [x] T017 [P] [US1] Write unit tests for console output formatter in `tests/test_output.c`

### Implementation for User Story 1

- [x] T018 [US1] Implement graph builder (place/route insertion, duplicate handling, adjacency lists) in `src/graph.c`
- [x] T019 [US1] Implement Dijkstra's algorithm with binary min-heap in `include/dijkstra.h` and `src/dijkstra.c`
- [x] T020 [US1] Implement Yen's k-shortest paths (k=2) in `include/yen.h` and `src/yen.c`
- [x] T021 [US1] Implement console output formatter in `include/output.h` and `src/output.c`
- [x] T022 [US1] Implement interactive CLI main flow and error handling for missing files, invalid route syntax, unknown places, zero-cost routes, and disconnected routes in `src/main.c`, `include/cli.h`, and `src/cli.c`
- [x] T023 [US1] Write integration test for full user journey (valid inputs) in `tests/test_cli_integration.c`
- [x] T024 [P] [US1] Write focused unit tests for FR-010 error paths in `tests/test_cli_errors.c`

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Handle Weather Adjustments (Priority: P2)

**Goal**: Load a weather file that makes routes impassable or more expensive, and recalculate best and second-best routes using the adjusted effective costs.

**Independent Test**: Load a weather file that sets a walking multiplier to zero; walking routes exclude that edge while bus and car routes still produce valid results with adjusted costs.

### Tests for User Story 2 (REQUIRED) ⚠️

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [x] T025 [P] [US2] Write unit tests for weather application engine in `tests/test_weather.c`

### Implementation for User Story 2

- [x] T026 [US2] Implement weather application engine (multiplier application, zero-multiplier impassability, unknown route warnings) in `include/weather.h` and `src/weather.c`
- [x] T027 [US2] Write integration test for weather adjustments in `tests/test_weather_integration.c`

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - Continuously Query Multiple Routes (Priority: P3)

**Goal**: After the first calculation, allow the user to query a new origin-destination pair with the same files or load different files without restarting the program.

**Independent Test**: Complete a route query, choose `n` at the next-action prompt, enter a new origin and destination, and receive new results without the program terminating.

### Tests for User Story 3 (REQUIRED) ⚠️

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [x] T028 [P] [US3] Write unit tests for re-run loop state transitions in `tests/test_cli_loop.c`

### Implementation for User Story 3

- [x] T029 [US3] Implement re-run loop (`n` new query, `c` change files, `s` exit) in `src/cli.c`
- [x] T030 [US3] Write integration test for re-run loop in `tests/test_rerun_integration.c`

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [x] T031 [P] Run all validation scenarios from `quickstart.md`
- [x] T032 [P] Update `README.md` with build instructions, run instructions, and file format summaries
- [x] T033 [P] Verify clean compilation with `-Wall -Wextra -Wpedantic` and zero warnings
- [x] T034 [P] Verify all dynamic memory is freed (valgrind, AddressSanitizer, or equivalent)
- [x] T035 Final acceptance test against success criteria SC-001 through SC-006 in `spec.md`
- [x] T036 [P] Create maximum-size performance fixture (50 places, 200 routes) in `tests/fixtures/performance_routes.txt`
- [x] T037 Add performance benchmark to `Makefile` that verifies SC-001 (parse <1s) and SC-002 (compute <2s)
- [x] T038 Integrate `gcov` coverage reporting into `Makefile` and verify SC-005 (100% coverage of parser, graph, Dijkstra, Yen modules)

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational phase completion
  - User stories can proceed in priority order (P1 → P2 → P3)
  - Some story-specific work can overlap once dependencies are met
- **Polish (Final Phase)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2). No dependencies on other stories. This is the MVP.
- **User Story 2 (P2)**: Can start after Foundational (Phase 2). No dependency on US1; weather adjustments only require the parser and graph from the Foundational phase.
- **User Story 3 (P3)**: Can start after Foundational (Phase 2) and US1 CLI implementation (T022) because the re-run loop extends the base interactive flow.

### Within Each User Story

- Tests MUST be written and FAIL before implementation
- Foundational data structures before story-specific services
- Services/endpoints before integration
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel (within Phase 2)
- All unit tests for a user story marked [P] can run in parallel
- Graph, Dijkstra, Yen, and output implementations in US1 can be developed in parallel once the parser and graph structure are ready

---

## Parallel Example: User Story 1

```bash
# Launch all unit tests for User Story 1 together:
Task: "Write unit tests for graph builder in tests/test_graph.c"
Task: "Write unit tests for Dijkstra's algorithm in tests/test_dijkstra.c"
Task: "Write unit tests for Yen's k-shortest paths in tests/test_yen.c"
Task: "Write unit tests for console output formatter in tests/test_output.c"

# Launch algorithm and output implementations in parallel once graph is ready:
Task: "Implement graph builder in src/graph.c"
Task: "Implement Dijkstra's algorithm with binary min-heap in include/dijkstra.h and src/dijkstra.c"
Task: "Implement Yen's k-shortest paths (k=2) in include/yen.h and src/yen.c"
Task: "Implement console output formatter in include/output.h and src/output.c"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1
4. **STOP and VALIDATE**: Run `make test` and the quickstart scenario
5. Demo the MVP

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Demo (MVP!)
3. Add User Story 2 → Test weather adjustments independently → Demo
4. Add User Story 3 → Test re-run loop independently → Demo
5. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 1 (graph, algorithms, CLI, output)
   - Developer B: User Story 2 (weather engine and tests)
   - Developer C: User Story 3 (re-run loop and tests)
3. Stories complete and integrate independently

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Verify tests fail before implementing
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- Avoid: vague tasks, same file conflicts, cross-story dependencies that break independence
