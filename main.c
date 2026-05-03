
#include <stdio.h>
#include <unistd.h> 
#include "os.h"
#include "TemperatureSensor.h"
#include "MotorCtrl.h"
#include "GPIO.h"
#include "I2C.h"
#include "Test_Motor.h"


#define OS_MANUAL

int main() {
#if defined(OS_MANUAL)
    os_init(OS_MODE_MANUAL);
#else
    os_init(OS_MODE_ASYNC);
#endif
    I2C_init();
    GPIO_init();
    TemperatureSensor_init();
    MotorCtrl_init();

#if defined(OS_MANUAL)
    test_motor_run();
#else
    while (1) {
        sleep(1);
    }
#endif
    return 0;
}