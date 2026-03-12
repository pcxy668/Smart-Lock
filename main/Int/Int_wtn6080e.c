#include "Int_wtn6080e.h"

void Int_WTN6080E_Init(void)
{
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = WTN6080E_DATA_SEL;
    gpio_config(&io_conf);

    gpio_set_level(WTN6080E_DATA_PIN, 1);
    DelayMs(200);
}

void Int_WTN6080E_Play(uint8_t byte)
{
    gpio_set_level(WTN6080E_DATA_PIN, 0);
    DelayMs(10);

    for (uint8_t i = 0; i < 8; i++)
    {
        if (byte & 0x01)
        {
            gpio_set_level(WTN6080E_DATA_PIN, 1);
            DelayUs(600);
            gpio_set_level(WTN6080E_DATA_PIN, 0);
            DelayUs(200);
        }
        else
        {
            gpio_set_level(WTN6080E_DATA_PIN, 1);
            DelayUs(200);
            gpio_set_level(WTN6080E_DATA_PIN, 0);
            DelayUs(600);            
        }
        byte >>= 1;
    }
    gpio_set_level(WTN6080E_DATA_PIN, 1);
    DelayMs(20);
}
