

#include <stdio.h>
#include "os.h"
#include "TemperatureSensor.h"

static void update(void *arg);
static const int UPDATE_INTERVAL = 1000; // in milliseconds

void TemperatureSensor_init()
{
    printf("Temperature sensor initialized.\n");
    os_schedule(UPDATE_INTERVAL, update, NULL);
}


static void update(void *arg)
{
    printf("Updating temperature sensor...\n");
    os_schedule(UPDATE_INTERVAL, update, arg);
}