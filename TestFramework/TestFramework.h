#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdbool.h>

typedef struct {
    int total;
    int passed;
    int failed;
} TestStats;

extern TestStats test_stats;

void test_assert(bool condition, const char* msg);
void test_report(void);

#endif




