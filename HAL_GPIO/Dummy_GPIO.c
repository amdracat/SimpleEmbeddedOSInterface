

#include <stdio.h>
#include "os.h"
#include "GPIO.h"

uint8_t dummy_gpio_state[16] = {0}; // ダミーGPIO状態

void GPIO_init()
{
}
void GPIO_write(uint8_t pin, uint8_t value)
{
    dummy_gpio_state[pin] = value;
}
uint8_t GPIO_read(uint8_t pin)
{
    return dummy_gpio_state[pin]; // ダミーデータ
}