

#include <stdio.h>
#include "os.h"
#include "I2C.h"

void I2C_init()
{
    printf("I2C initialized.\n");
}
void I2C_write(uint8_t address, uint8_t *data, size_t length)
{
    printf("Writing to I2C device at address 0x%02X\n", address);
}
void I2C_read(uint8_t address, uint8_t *buffer, size_t length)
{
    printf("Reading from I2C device at address 0x%02X\n", address);
}