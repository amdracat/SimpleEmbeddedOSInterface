#ifndef I2C_H
#define I2C_H

void I2C_init();
void I2C_write(uint8_t address, uint8_t *data, size_t length);
void I2C_read(uint8_t address, uint8_t *buffer, size_t length);

#endif // I2C_H