#ifndef GPIO_H
#define GPIO_H

void GPIO_init();
void GPIO_write(uint8_t pin, uint8_t value);
uint8_t GPIO_read(uint8_t pin);


#endif // GPIO_H