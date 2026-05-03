

#include <stdio.h>
#include "os.h"
#include "I2C.h"

uint8_t dummy_i2c_data[16] = {0}; // ダミーI2Cデータ

void I2C_init()
{

}
void I2C_write(uint8_t address, uint8_t *data, size_t length)
{
    for (size_t i = 0; i < length && i < sizeof(dummy_i2c_data); i++) {
        dummy_i2c_data[i] = data[i];
    }
}
void I2C_read(uint8_t address, uint8_t *buffer, size_t length)
{
    for (size_t i = 0; i < length && i < sizeof(dummy_i2c_data); i++) {
        buffer[i] = dummy_i2c_data[i];
    }
}