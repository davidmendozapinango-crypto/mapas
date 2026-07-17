#ifndef UNITY_H
#define UNITY_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void unity_begin(void);
int unity_end(void);
void setUp(void);
void tearDown(void);

#define RUN_TEST(func) run_test(#func, func)

typedef void (*unity_test_func_t)(void);

void run_test(const char *name, unity_test_func_t func);

void unity_assert_fail(const char *file, int line, const char *msg);
void unity_assert_equal_int(int expected, int actual, const char *file, int line);
void unity_assert_equal_double(double expected, double actual, double precision, const char *file, int line);
void unity_assert_equal_string(const char *expected, const char *actual, const char *file, int line);
void unity_assert_null(const void *p, const char *file, int line);
void unity_assert_not_null(const void *p, const char *file, int line);

#define TEST_ASSERT(cond) do { if (!(cond)) unity_assert_fail(__FILE__, __LINE__, #cond); } while (0)
#define TEST_ASSERT_TRUE(cond) TEST_ASSERT(cond)
#define TEST_ASSERT_FALSE(cond) TEST_ASSERT(!(cond))
#define TEST_ASSERT_EQUAL_INT(expected, actual) unity_assert_equal_int((expected), (actual), __FILE__, __LINE__)
#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual) unity_assert_equal_double((expected), (actual), 0.001, __FILE__, __LINE__)
#define TEST_ASSERT_EQUAL_STRING(expected, actual) unity_assert_equal_string((expected), (actual), __FILE__, __LINE__)
#define TEST_ASSERT_NULL(p) unity_assert_null((p), __FILE__, __LINE__)
#define TEST_ASSERT_NOT_NULL(p) unity_assert_not_null((p), __FILE__, __LINE__)

#endif
