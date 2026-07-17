#include "unity.h"

void run_parser_tests(void);
void run_graph_tests(void);
void run_dijkstra_tests(void);
void run_yen_tests(void);
void run_weather_tests(void);
void run_output_tests(void);
void run_cli_errors_tests(void);
void run_cli_loop_tests(void);
void run_cli_integration_tests(void);
void run_weather_integration_tests(void);
void run_rerun_integration_tests(void);

void setUp(void) { }
void tearDown(void) { }

int main(void)
{
    unity_begin();

    run_parser_tests();
    run_graph_tests();
    run_dijkstra_tests();
    run_yen_tests();
    run_weather_tests();
    run_output_tests();
    run_cli_errors_tests();
    run_cli_loop_tests();
    run_cli_integration_tests();
    run_weather_integration_tests();
    run_rerun_integration_tests();

    return unity_end();
}
