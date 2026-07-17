#include "unity.h"
#include "cli.h"
#include <stdio.h>
#include <string.h>

void run_cli_integration_tests(void);

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

static void test_valid_journey(void)
{
    const char *input =
        "tests/fixtures/valid_routes.txt\n"
        "\n"
        "A\n"
        "D\n"
        "s\n";
    FILE *in = make_input(input, "cli_input.txt");
    FILE *out = make_output("cli_output.txt");
    TEST_ASSERT_NOT_NULL(in);
    TEST_ASSERT_NOT_NULL(out);

    cli_run_with_streams(in, out);

    char buf[2048];
    read_output(out, buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "A -> B -> C -> D"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Optima"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Alternativa"));

    fclose(in);
    fclose(out);
}

void run_cli_integration_tests(void)
{
    RUN_TEST(test_valid_journey);
}
