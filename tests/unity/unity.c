#include "unity.h"

static int unity_tests_run = 0;
static int unity_tests_failed = 0;
static const char *unity_current_test = NULL;

void unity_begin(void) {
    unity_tests_run = 0;
    unity_tests_failed = 0;
}

int unity_end(void) {
    printf("\n-----------------------\n");
    printf("%d Tests %d Failures\n", unity_tests_run, unity_tests_failed);
    printf("-----------------------\n");
    return unity_tests_failed;
}

void run_test(const char *name, unity_test_func_t func) {
    unity_current_test = name;
    setUp();
    func();
    tearDown();
    unity_tests_run++;
    printf("PASS: %s\n", name);
}

void unity_assert_fail(const char *file, int line, const char *msg) {
    unity_tests_failed++;
    printf("FAIL: %s at %s:%d (%s)\n", unity_current_test, file, line, msg);
}

void unity_assert_equal_int(int expected, int actual, const char *file, int line) {
    if (expected != actual) {
        unity_tests_failed++;
        printf("FAIL: %s at %s:%d Expected %d, got %d\n", unity_current_test, file, line, expected, actual);
    }
}

void unity_assert_equal_double(double expected, double actual, double precision, const char *file, int line) {
    if (fabs(expected - actual) > precision) {
        unity_tests_failed++;
        printf("FAIL: %s at %s:%d Expected %f, got %f\n", unity_current_test, file, line, expected, actual);
    }
}

void unity_assert_equal_string(const char *expected, const char *actual, const char *file, int line) {
    if ((expected == NULL && actual != NULL) || (expected != NULL && actual == NULL) ||
        (expected != NULL && actual != NULL && strcmp(expected, actual) != 0)) {
        unity_tests_failed++;
        printf("FAIL: %s at %s:%d Expected \"%s\", got \"%s\"\n", unity_current_test, file, line,
               expected ? expected : "(null)", actual ? actual : "(null)");
    }
}

void unity_assert_null(const void *p, const char *file, int line) {
    if (p != NULL) {
        unity_tests_failed++;
        printf("FAIL: %s at %s:%d Expected NULL, got non-NULL\n", unity_current_test, file, line);
    }
}

void unity_assert_not_null(const void *p, const char *file, int line) {
    if (p == NULL) {
        unity_tests_failed++;
        printf("FAIL: %s at %s:%d Expected non-NULL, got NULL\n", unity_current_test, file, line);
    }
}
