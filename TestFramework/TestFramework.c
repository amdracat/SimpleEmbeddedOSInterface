#include "TestFramework.h"
#include <stdio.h>

TestStats test_stats = {0, 0, 0};

void test_assert(bool condition, const char* msg) {
    test_stats.total++;
    if (condition) {
        test_stats.passed++;
        //printf("PASS: %s\n", msg);
    } else {
        test_stats.failed++;
        printf("FAIL: %s\n", msg);
    }
}

void test_report(void) {
    printf("\n=== Test Report ===\n");
    printf("Total: %d\n", test_stats.total);
    printf("Passed: %d\n", test_stats.passed);
    printf("Failed: %d\n", test_stats.failed);
    if (test_stats.failed == 0) {
        printf("All tests passed!\n");
    } else {
        printf("Some tests failed.\n");
    }
}




