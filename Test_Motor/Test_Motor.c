
#include <stdio.h>
#include "MotorCtrl.h"
#include "Test_Motor.h"
#include "TestFramework.h"
#include "os.h"
#include "TemperatureSensor.h"
#include "GPIO.h"
#include "I2C.h"

void test_motor_run()
{
    printf("=== Motor Control Test Suite ===\n\n");
    
    // 初期化は main.c で既に実施されています
    
    // テスト1: モーター速度の直接設定
    {
        printf("--- Test 1: Direct Speed Setting ---\n");
        motor_speed_t speed;
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_run_all();
        MotorCtrl_get_speed(&speed);
        test_assert(speed == MOTOR_SPEED_HIGH, "Motor speed set to HIGH");
        
        MotorCtrl_set_speed(MOTOR_SPEED_LOW);
        os_run_all();
        MotorCtrl_get_speed(&speed);
        test_assert(speed == MOTOR_SPEED_LOW, "Motor speed set to LOW");
        
        MotorCtrl_set_speed(MOTOR_SPEED_STOP);
        os_run_all();
        MotorCtrl_get_speed(&speed);
        test_assert(speed == MOTOR_SPEED_STOP, "Motor speed set to STOP");
    }

    // テスト2: 温度が-30度未満 → モーター停止
    {
        printf("--- Test 2: Temperature < -30[degC](Motor OFF) ---\n");
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_run_all();

        // I2C経由で-40度を設定: -40 * 16 = -640 = 0xFD80
        uint8_t temp_data[2] = {0x80, 0xFD};
        I2C_write(0x48, temp_data, 2);
        
        // 初回のmotorctrl(@200ms)と temperature update(@1000ms)を実行
        os_advance_time(1000);
        os_run_all();
        
        // motorctrlが温度更新を反映させるため、次のmotorctrl実行まで進める
        os_advance_time(200);  // motorctrl@1200ms 実行
        os_run_all();
        
        uint8_t enable = GPIO_read(0);
        uint8_t speed_pin = GPIO_read(1);
        
        test_assert(enable == 0, "Motor disabled at -40°C");
        test_assert(speed_pin == 0, "Motor high-speed disabled at -40°C");
    }

    // テスト3: 温度が-30～30度 → モーター指示値に応じて高速/低速/停止
    {
        printf("--- Test 3: Temperature -30[degC]to 30[degC](Motor HIGH SPEED) ---\n");
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_run_all();

        // 10度を設定: 10 * 16 = 160 = 0x00A0
        uint8_t temp_data[2] = {0xA0, 0x00};
        I2C_write(0x48, temp_data, 2);
        
        os_advance_time(1000);
        os_run_all();
        os_advance_time(200);
        os_run_all();
        
        uint8_t enable = GPIO_read(0);
        uint8_t speed_pin = GPIO_read(1);
        
        test_assert(enable == 1, "Motor enabled at 10°C");
        test_assert(speed_pin == 1, "Motor high-speed enabled at 10°C");

        MotorCtrl_set_speed(MOTOR_SPEED_LOW);
        os_run_all();

        os_advance_time(200);
        os_run_all();

        enable = GPIO_read(0);
        speed_pin = GPIO_read(1);

        test_assert(enable == 1, "Motor enabled at 10°C");
        test_assert(speed_pin == 0, "Motor low-speed enabled at 10°C");


        MotorCtrl_set_speed(MOTOR_SPEED_STOP);
        os_run_all();

        os_advance_time(200);
        os_run_all();

        enable = GPIO_read(0);
        speed_pin = GPIO_read(1);

        test_assert(enable == 0, "Motor disabled at 10°C");
        test_assert(speed_pin == 0, "Motor low-speed disabled at 10°C");

    }

    // テスト4: 温度が30～50度 → モーター低速
    {
        printf("--- Test 4: Temperature 30[degC]to 50[degC](Motor LOW SPEED) ---\n");
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_run_all();

        // 40度を設定: 40 * 16 = 640 = 0x0280
        uint8_t temp_data[2] = {0x80, 0x02};
        I2C_write(0x48, temp_data, 2);
        
        os_advance_time(1000);
        os_run_all();
        os_advance_time(200);
        os_run_all();
        
        uint8_t enable = GPIO_read(0);
        uint8_t speed_pin = GPIO_read(1);
        
        test_assert(enable == 1, "Motor enabled at 40°C");
        test_assert(speed_pin == 0, "Motor high-speed disabled at 40[degC](low speed)");

    }

    // テスト5: 温度が50度以上 → モーター停止
    {
        printf("--- Test 5: Temperature >= 50[degC](Motor OFF) ---\n");
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_run_all();

        // 60度を設定: 60 * 16 = 960 = 0x03C0
        uint8_t temp_data[2] = {0xC0, 0x03};
        I2C_write(0x48, temp_data, 2);
        
        os_advance_time(1000);
        os_run_all();
        os_advance_time(200);
        os_run_all();
        
        uint8_t enable = GPIO_read(0);
        uint8_t speed_pin = GPIO_read(1);
        
        test_assert(enable == 0, "Motor disabled at 60°C");
        test_assert(speed_pin == 0, "Motor high-speed disabled at 60°C");
    }

    // テスト結果レポート
    test_report();
}


