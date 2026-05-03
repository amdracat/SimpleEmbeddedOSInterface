#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H


typedef enum {
    MOTOR_SPEED_STOP,
    MOTOR_SPEED_LOW,
    MOTOR_SPEED_HIGH
} motor_speed_t;
void MotorCtrl_init();
void MotorCtrl_set_speed(motor_speed_t speed);
void MotorCtrl_get_speed(motor_speed_t *speed);

#endif // MOTOR_CTRL_H