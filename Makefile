.PHONY: all test clean coverage

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude -Itests/unity
LDFLAGS = -lm

SRC = src/main.c src/cli.c src/parser.c src/graph.c src/dijkstra.c src/yen.c src/weather.c src/output.c
OBJ = $(patsubst src/%.c,build/%.o,$(SRC))
APP = build/mapas

UNITY = tests/unity/unity.c

TEST_SRC = tests/test_parser.c tests/test_graph.c tests/test_dijkstra.c tests/test_yen.c tests/test_weather.c tests/test_output.c tests/test_cli_errors.c tests/test_cli_loop.c tests/test_cli_integration.c tests/test_weather_integration.c tests/test_rerun_integration.c
TEST_OBJ = $(patsubst tests/%.c,build/tests/%.o,$(TEST_SRC))
TEST_BIN = build/test_runner

all: $(APP)

$(APP): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c -o $@ $<

build/tests/%.o: tests/%.c | build/tests
	$(CC) $(CFLAGS) -c -o $@ $<

$(TEST_BIN): $(filter-out build/main.o,$(OBJ)) $(TEST_OBJ) build/tests/unity.o build/tests/test_runner.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/tests/test_runner.o: tests/test_runner.c | build/tests
	$(CC) $(CFLAGS) -c -o $@ $<

build/tests/unity.o: tests/unity/unity.c | build/tests
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(APP) $(TEST_BIN)
	./$(TEST_BIN)

coverage: CFLAGS += -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -fprofile-arcs -ftest-coverage
coverage: clean $(TEST_BIN)
	./$(TEST_BIN)
	gcov -o build src/parser.c src/graph.c src/dijkstra.c src/yen.c
	gcov -o build/tests tests/test_parser.c tests/test_graph.c tests/test_dijkstra.c tests/test_yen.c

clean:
	rm -rf build/*
	rm -f *.gcov *.gcda *.gcno src/*.gcno src/*.gcda tests/*.gcno tests/*.gcda tests/unity/*.gcno tests/unity/*.gcda

build:
	mkdir -p build

build/tests:
	mkdir -p build/tests
