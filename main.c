
#include <stdio.h>
#include <unistd.h> 
#include "os.h"
#include "TemperatureSensor.h"

#define EVENT_TEST 1

void on_event(void *arg) {
    printf("event received: %d\n", (int)(intptr_t)arg);
}

void test_job(void *arg) {
    printf("job executed: %s\n", (char*)arg);
}

int main() {
    os_init();

    TemperatureSensor_init();
    os_event_subscribe(EVENT_TEST, on_event, (void*)123);

    os_post(test_job, "hello");

    os_event_publish(EVENT_TEST);

    os_schedule(1000, test_job, "delayed");

    while (1) {
        sleep(1);
    }

    return 0;
}