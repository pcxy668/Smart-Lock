#include "Int_sc12b.h"

// 初始化SC12B相关GPIO引脚
void Int_SC12B_Init(void)
{
    gpio_config_t io_conf = {};

    // 禁用中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // 默认输出模式
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = IIC_SDA_SEL | IIC_SCL_SEL;
    // 使以上配置生效
    gpio_config(&io_conf);

    // 上升沿中断
    // posedge ==> positive edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = SC12B_INT_SEL;
    gpio_config(&io_conf); 

    DelayMs(300);
}

// 从SC12B简易读取按键值
uint16_t Int_SC12B_SimpleRead(void)
{
    uint8_t buffer1 = 0,buffer2 = 0;

    IIC_Start();
    if (IIC_SendByteAndGetNACK((SC12B_ADDR << 1) | 0x01))
    {
        IIC_Stop();
        return 0xFF;    
    }
    buffer1 = IIC_ReceiveByte();
    IIC_Respond(0);
    buffer2 = IIC_ReceiveByte();
    IIC_Respond(1);
    IIC_Stop();

    return ((uint16_t)buffer1 << 8) | buffer2;
}

// 将SC12B简易读取按键值转换为实际对应的按键值
int8_t Int_SC12B_GetKeyValue(void)
{
    uint16_t key_content = Int_SC12B_SimpleRead();
    if (key_content == 0x8000)
        return 0; // 1键
    if (key_content == 0x4000)
        return 1; // 2键
    if (key_content == 0x2000)
        return 2; // 3键
    if (key_content == 0x1000)
        return 3; // 4键
    if (key_content == 0x800)
        return 4; // 5键
    if (key_content == 0x400)
        return 5; // 6键
    if (key_content == 0x200)
        return 6; // 7键
    if (key_content == 0x100)
        return 7; // 8键
    if (key_content == 0x80)
        return 8; // 9键
    if (key_content == 0x40)
        return 9;  // #键
    if (key_content == 0x20)
        return -1; // 0
    if (key_content == 0x10)
        return 11; // M键
    return 0x0F;
}
