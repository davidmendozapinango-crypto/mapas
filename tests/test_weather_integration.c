#include "unity.h"
#include "cli.h"
#include <stdio.h>
#include <string.h>

void run_weather_integration_tests(void);

static FILE *make_input(const char *text, const char *name)
{
    char path[256];
    snprintf(path, sizeof(path), "tests/fixtures/%s", name);
    FILE *f = fopen(path, "w");
    if (f) {
        fprintf(f, "%s", text);
        fclose(f);
        return fopen(path, "r");
    }
    return NULL;
}

static FILE *make_output(const char *name)
{
    char path[256];
    snprintf(path, sizeof(path), "tests/fixtures/%s", name);
    return fopen(path, "w+");
}

static void read_output(FILE *f, char *buf, size_t size)
{
    rewind(f);
    size_t n = fread(buf, 1, size - 1, f);
    buf[n] = '\0';
}

static void test_weather_excludes_walking_edge(void)
{
    const char *input =
        "tests/fixtures/valid_routes.txt\n"
        "tests/fixtures/weather_walk_zero.txt\n"
        "A\n"
        "D\n"
        "s\n";
    FILE *in = make_input(input, "weather_input.txt");
    FILE *out = make_output("weather_output.txt");
    TEST_ASSERT_NOT_NULL(in);
    TEST_ASSERT_NOT_NULL(out);

    cli_run_with_streams(in, out);

    char buf[2048];
    read_output(out, buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "A -> C -> D"));

    fclose(in);
    fclose(out);
}

void run_weather_integration_tests(void)
{
    RUN_TEST(test_weather_excludes_walking_edge);
}
