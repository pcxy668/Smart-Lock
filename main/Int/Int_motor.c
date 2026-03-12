#include "Int_motor.h"

void Int_Motor_Init(void)
{
    gpio_config_t io_conf = {};

    // 禁用中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // 默认输出模式
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << MOTOR_IN1_PIN) | (1ULL << MOTOR_IN2_PIN);
    // 使以上配置生效
    gpio_config(&io_conf);

    gpio_set_level(MOTOR_IN1_PIN, 0);
    gpio_set_level(MOTOR_IN2_PIN, 0);

    DelayMs(10);    
}

void Int_Motor_OpenLock(void)
{
    //正转 2s
    gpio_set_level(MOTOR_IN1_PIN, 0);
    gpio_set_level(MOTOR_IN2_PIN, 1);
    DelayMs(2000);
    
    //停止 1s
    gpio_set_level(MOTOR_IN1_PIN, 0);
    gpio_set_level(MOTOR_IN2_PIN, 0);
    DelayMs(1000);

    //反转 1s
    gpio_set_level(MOTOR_IN1_PIN, 1);
    gpio_set_level(MOTOR_IN2_PIN, 0);
    DelayMs(2000);

    //停止
    gpio_set_level(MOTOR_IN1_PIN, 0);
    gpio_set_level(MOTOR_IN2_PIN, 0);
}
