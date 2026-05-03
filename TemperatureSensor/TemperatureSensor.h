#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#define TEMPERATURE_UPDATE_EVENT 1

void TemperatureSensor_init();
int TemperatureSensor_get_temperature();
#endif // TEMPERATURE_SENSOR_H