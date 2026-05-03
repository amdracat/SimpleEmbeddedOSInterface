

#include <stdio.h>
#include "os.h"
#include "MotorCtrl.h"
#include "TemperatureSensor.h"
#include "GPIO.h"

static int current_temperature = 0;
static motor_speed_t motor_speed = 0;
static const int UPDATE_INTERVAL = 200; // in milliseconds

static uint8_t PORT_ENABLE_MOTOR = 0;
static uint8_t PORT_HIGH_SPEED = 1;

static void motorctrl(void *arg);
static void setspeed(void *arg);
static int calculate_motor_speed(int temperature);
static void temperature_update_handler(void *arg) ;

void MotorCtrl_init()
{
    GPIO_init();
    os_event_subscribe(TEMPERATURE_UPDATE_EVENT, temperature_update_handler, NULL);
    os_schedule(UPDATE_INTERVAL, motorctrl, NULL);
}
void MotorCtrl_set_speed(motor_speed_t speed)
{
    os_post(setspeed, (void*)(intptr_t)speed);
    
}
void MotorCtrl_get_speed(motor_speed_t *speed)
{
    *speed = motor_speed;
}
static void setspeed(void *arg)
{
    motor_speed = (motor_speed_t)(intptr_t)arg;
}
static int calculate_motor_speed(int temperature)
{
    /* -30度以下は停止、-30からから30度未満は高速、30度から50度未満は低速、それ以上は停止 */
    if (temperature < -30) {
        return MOTOR_SPEED_STOP; // 停止
    } else if (temperature < 30) {
        /* 常温では指定可能 */
        if (motor_speed == MOTOR_SPEED_HIGH) {
            return MOTOR_SPEED_HIGH; // 既に高速なら維持
        }else if (motor_speed == MOTOR_SPEED_LOW) {
            return MOTOR_SPEED_LOW; // 既に低速なら維持
        } else {
            return MOTOR_SPEED_STOP; // それ以外は停止
        }
    } else if (temperature < 50) {
        /* 低温では低速以下 */
        if (motor_speed == MOTOR_SPEED_HIGH) {
            return MOTOR_SPEED_LOW; // 高速から低速に変更
        } else if (motor_speed == MOTOR_SPEED_LOW) {
            return MOTOR_SPEED_LOW; // 既に低速なら維持
        } else {
            return MOTOR_SPEED_STOP; // それ以外は停止
        }
    } else {
        return MOTOR_SPEED_STOP; // 停止
    }
}
static void temperature_update_handler(void *arg) {
    int temp = TemperatureSensor_get_temperature();
    current_temperature = temp;
}

static void motorctrl(void *arg)
{
    (void)arg;
    // 温度に応じてモーター速度を調整する例
    int speed = calculate_motor_speed(current_temperature);
    if (speed == 2) {
        GPIO_write(PORT_ENABLE_MOTOR, 1); // モーターON
        GPIO_write(PORT_HIGH_SPEED, 1); // 高速ON
    } else if (speed == 1) {
        GPIO_write(PORT_ENABLE_MOTOR, 1); // モーターON
        GPIO_write(PORT_HIGH_SPEED, 0); // 高速OFF
    } else {
        GPIO_write(PORT_ENABLE_MOTOR, 0); // モーターOFF
        GPIO_write(PORT_HIGH_SPEED, 0); // 高速OFF
    }
    printf("MotorCtrl: Temperature=%d[degC], Speed=%d, Enable=%d, HighSpeed=%d\n", current_temperature, speed, GPIO_read(PORT_ENABLE_MOTOR), GPIO_read(PORT_HIGH_SPEED));
    os_schedule(UPDATE_INTERVAL, motorctrl, arg);
}