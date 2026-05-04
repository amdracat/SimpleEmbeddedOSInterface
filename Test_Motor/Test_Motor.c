
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
        os_sim_run_all();
        MotorCtrl_get_speed(&speed);
        test_assert(speed == MOTOR_SPEED_HIGH, "Motor speed set to HIGH");
        
        MotorCtrl_set_speed(MOTOR_SPEED_LOW);
        os_sim_run_all();
        MotorCtrl_get_speed(&speed);
        test_assert(speed == MOTOR_SPEED_LOW, "Motor speed set to LOW");
        
        MotorCtrl_set_speed(MOTOR_SPEED_STOP);
        os_sim_run_all();
        MotorCtrl_get_speed(&speed);
        test_assert(speed == MOTOR_SPEED_STOP, "Motor speed set to STOP");
    }

    // テスト2: 温度が-10度未満 → モーター停止
    {
        printf("--- Test 2: Temperature < -10[degC](Motor OFF) ---\n");
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_sim_run_all();

        // I2C経由で-15度を設定: -15 * 16 = -240 = 0xFE00
        uint8_t temp_data[2] = {0x00, 0xFE};
        I2C_write(0x48, temp_data, 2);
        
        // 初回のmotorctrl(@200ms)と temperature update(@1000ms)を実行
        os_sim_advance_time(1000);
        os_sim_run_all();
        
        // motorctrlが温度更新を反映させるため、次のmotorctrl実行まで進める
        os_sim_advance_time(200);  // motorctrl@1200ms 実行
        os_sim_run_all();
        
        uint8_t enable = GPIO_read(0);
        uint8_t speed_pin = GPIO_read(1);
        
        test_assert(enable == 0, "Motor disabled at -10[degC]");
        test_assert(speed_pin == 0, "Motor high-speed disabled at -10[degC]");
    }

    // テスト3: 温度が-10～40度 → モーター指示値に応じて高速/低速/停止
    {
        printf("--- Test 3: Temperature -10[degC]to 40[degC](Motor HIGH SPEED) ---\n");
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_sim_run_all();

        // 35度を設定: 35 * 16 = 560 = 0x0220
        uint8_t temp_data[2] = {0x20, 0x02};
        I2C_write(0x48, temp_data, 2);
        
        os_sim_advance_time(1000);
        os_sim_run_all();
        os_sim_advance_time(200);
        os_sim_run_all();
        
        uint8_t enable = GPIO_read(0);
        uint8_t speed_pin = GPIO_read(1);
        
        test_assert(enable == 1, "Motor enabled at 40[degC]");
        test_assert(speed_pin == 1, "Motor high-speed enabled at 40[degC]");

        MotorCtrl_set_speed(MOTOR_SPEED_LOW);
        os_sim_run_all();

        os_sim_advance_time(200);
        os_sim_run_all();

        enable = GPIO_read(0);
        speed_pin = GPIO_read(1);

        test_assert(enable == 1, "Motor enabled at 40[degC]");
        test_assert(speed_pin == 0, "Motor low-speed enabled at 40[degC]");


        MotorCtrl_set_speed(MOTOR_SPEED_STOP);
        os_sim_run_all();

        os_sim_advance_time(200);
        os_sim_run_all();

        enable = GPIO_read(0);
        speed_pin = GPIO_read(1);

        test_assert(enable == 0, "Motor disabled at 40[degC]");
        test_assert(speed_pin == 0, "Motor low-speed disabled at 40[degC]");

    }

    // テスト4: 温度が40～60度 → モーター低速
    {
        printf("--- Test 4: Temperature 40[degC]to 60[degC](Motor LOW SPEED) ---\n");
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_sim_run_all();

        // 45度を設定: 45 * 16 = 720 = 0x02D0
        uint8_t temp_data[2] = {0xD0, 0x02};
        I2C_write(0x48, temp_data, 2);
        
        os_sim_advance_time(1000);
        os_sim_run_all();
        os_sim_advance_time(200);
        os_sim_run_all();
        
        uint8_t enable = GPIO_read(0);
        uint8_t speed_pin = GPIO_read(1);
        
        test_assert(enable == 1, "Motor enabled at 45[degC]");
        test_assert(speed_pin == 0, "Motor high-speed disabled at 45[degC](low speed)");

    }

    // テスト5: 温度が60度以上 → モーター停止
    {
        printf("--- Test 5: Temperature >= 60[degC](Motor OFF) ---\n");
        
        MotorCtrl_set_speed(MOTOR_SPEED_HIGH);
        os_sim_run_all();

        // 70度を設定: 70 * 16 = 1120 = 0x0460
        uint8_t temp_data[2] = {0x60, 0x04};
        I2C_write(0x48, temp_data, 2);
        
        os_sim_advance_time(1000);
        os_sim_run_all();
        os_sim_advance_time(200);
        os_sim_run_all();
        
        uint8_t enable = GPIO_read(0);
        uint8_t speed_pin = GPIO_read(1);
        
        test_assert(enable == 0, "Motor disabled at 70[degC]");
        test_assert(speed_pin == 0, "Motor high-speed disabled at 70[degC]");
    }

    // テスト結果レポート
    test_report();
}


