

#include <stdio.h>
#include "os.h"
#include "TemperatureSensor.h"
#include "I2C.h"

static void update(void *arg);
static const int UPDATE_INTERVAL = 1000; // in milliseconds
int current_temperature = 0;
static void settemperature(uint8_t* temp);



void TemperatureSensor_init()
{
    I2C_init();
    os_schedule(UPDATE_INTERVAL, update, NULL);
}
int TemperatureSensor_get_temperature()
{
    return current_temperature;
}


static void update(void *arg)
{
    uint8_t buffer[2];
    I2C_read(0x48, buffer, sizeof(buffer));
    settemperature(buffer);
    os_event_publish(TEMPERATURE_UPDATE_EVENT);
    os_schedule(UPDATE_INTERVAL, update, arg);
}

static void settemperature(uint8_t* temp)
{
    uint16_t  tmp = temp[0] | (0xFF00 & (temp[1] << 8));
    int16_t temperature = (int16_t)tmp;
    temperature = temperature >> 4; // 4ビット右シフトして温度値を取得
    current_temperature = temperature;
    printf("Temperature updated: %d[degC]\n", current_temperature);
}