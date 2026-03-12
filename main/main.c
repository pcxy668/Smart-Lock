#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "Int_sc12b.h"
#include "Int_wtn6080e.h"
#include "Int_ws2812b.h"
#include "Int_fpm383.h"
#include "Int_bluetooth.h"
#include "Int_wifi.h"
#include "Int_tempPassword.h"
#include "Int_ota.h"
#include "Int_motor.h"

#define ESP_INTR_FLAG_DEFAULT 0

// 是否有手指按在指纹模块上面
static uint8_t is_finger_on = 0;

// 是否是录入指纹任务
static uint8_t is_enroll_task = 0;

// 保存按键数组
static int8_t key_value[6] = {0};

// 按键次数
static uint8_t key_count = 0;

// 是否开始OTA
static uint8_t is_ota = 0;

// 队列声明
static QueueHandle_t gpio_evt_queue = NULL;

// GPIO中断处理函数
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    if (gpio_num == SC12B_INT_PIN)
    {
        xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
    }
    else if(gpio_num == FPM383_INT_PIN)
    {
        is_finger_on = 1;

        // 当指纹模块产生中断时，暂时屏蔽之后来自指纹模块和电容键盘的中断
        gpio_isr_handler_remove(SC12B_INT_PIN);
        gpio_isr_handler_remove(FPM383_INT_PIN);
    }
}

// 重置指纹模块的状态
static void reset_finger_state(void) {
    // 清除标志位
    is_finger_on = 0;
    is_enroll_task = 0;

    // 等待指纹模块进入睡眠
    while (Int_FPM383_Sleep())
    DelayMs(100);

    // 恢复GPIO中断处理
    gpio_isr_handler_add(SC12B_INT_PIN, gpio_isr_handler, (void *)SC12B_INT_PIN);
    gpio_isr_handler_add(FPM383_INT_PIN, gpio_isr_handler, (void *)FPM383_INT_PIN);
}

// GPIO任务
static void gpio_task(void* arg)
{
    uint32_t io_num;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            int8_t keyValue = Int_SC12B_GetKeyValue();
            if (keyValue <= 11)
            {
                Int_WS2812B_Light(keyValue == -1 ? 10 : keyValue);
                printf("keyValue: %d\n", keyValue);

                key_value[key_count++] = keyValue;
                if (key_count >= 6)
                {
                    // 指纹秘钥 2017**
                    if (key_value[0] == 1 && key_value[1] == -1 && key_value[2] == 0 && key_value[3] == 6  && key_value[4] == 9  && key_value[5] == 9)
                    {
                        is_enroll_task = 1;
                        printf("请输入指纹\n");
                    }
                    // OTA秘钥 1970**
                    else if (key_value[0] == 0 && key_value[1] == 8 && key_value[2] == 6 && key_value[3] == -1  && key_value[4] == 9  && key_value[5] == 9)
                    {
                        is_ota = 1;
                        printf("开启OTA标志位\n");
                    }
                    // 门锁秘钥
                    else
                    {
                        nvs_handle_t my_handle;
                        int32_t pass = 0;
                        int32_t tmp = (key_value[0] + 1) * 100000 + 
                                      (key_value[1] + 1) * 10000 + 
                                      (key_value[2] + 1) * 1000 + 
                                      (key_value[3] + 1) * 100 + 
                                      (key_value[4] + 1) * 10 + 
                                      (key_value[5] + 1);
                        nvs_open("storage", NVS_READONLY, &my_handle);
                        nvs_get_i32(my_handle, "password", &pass);
                        nvs_close(my_handle);
                        if (tmp == pass)
                        {
                            Int_Motor_OpenLock();
                            printf("密码正确,已开锁\n");
                        }
                        else
                        {
                            if (!Int_TempPassword_Compare(tmp))
                            {
                                Int_Motor_OpenLock();
                                printf("临时密码正确,已开锁\n");
                            }
                            else
                            {
                                printf("tmp:%ld,pass:%ld,密码错误,请重新输入\n",tmp,pass);
                            }
                        }
                    }
                    key_count = 0;
                    memset(key_value,0,sizeof(key_value));
                }
                
                if (keyValue == 11)
                {
                    key_count = 0;
                    memset(key_value,0,sizeof(key_value));
                    printf("请输入密码：\n");
                }
                
            }        
        }
        DelayMs(50);
    }
}

// 录入指纹任务
static void enroll_task(void* arg)
{
    while (1)
    {
        if (is_finger_on && is_enroll_task)
        {
            if (!Int_FPM383_Enroll())
            {
                printf("录入指纹成功\n");
            } 
            else 
            {
                printf("录入指纹失败\n");
            }
            reset_finger_state();
        }
        DelayMs(3000);
    }
    
}

// 识别指纹任务
static void identify_task(void* arg)
{
    while (1)
    {
        if (is_finger_on && !is_enroll_task)
        {
            if(!Int_FPM383_Identify())
            {
                Int_Motor_OpenLock();
                printf("验证指纹成功\n");
            } 
            else
            {
                printf("验证指纹失败\n");
            }  
            reset_finger_state();
        }
        DelayMs(1000);
    }
}

static void OTA_task(void* arg)
{
    while (1)
    {
        if(is_ota){
            is_ota = 0;
            Int_OTA_Init();
        }
        DelayMs(5000);
    }
}

void app_main(void)
{
    printf("当前为APP程序1\n");

    // =====================    外设部分    =====================

    // WTN6080E初始化 由于喇叭模块的信号干扰，暂不使用
    //Int_WTN6080E_Init();
    // WS2812B初始化
    Int_WS2812B_Init();    
    
    // SC12B初始化
    Int_SC12B_Init();

    // FPM383初始化
    Int_FPM383_Init();

    // 蓝牙初始化
    Int_Bluetooth_Init();

    // Wifi初始化
    Int_WIFI_Init();

    // 电机初始化
    Int_Motor_Init();

    // =====================    中断管理    =====================
    // 开启中断服务
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // 注册中断处理函数
    gpio_isr_handler_add(SC12B_INT_PIN, gpio_isr_handler, (void*) SC12B_INT_PIN);
    gpio_isr_handler_add(FPM383_INT_PIN, gpio_isr_handler, (void*) FPM383_INT_PIN);

    // =====================    消息队列    =====================
    // 创建消息队列
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // =====================    任务调度    =====================
    // 创建GPIO任务
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);
    xTaskCreate(enroll_task, "enroll_task", 2048, NULL, 10, NULL);
    xTaskCreate(identify_task, "identify_task", 2048, NULL, 10, NULL);
    xTaskCreate(OTA_task, "OTA_task", 8192, NULL, 10, NULL);
}
