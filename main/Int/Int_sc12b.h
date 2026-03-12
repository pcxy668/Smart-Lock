#ifndef __INT_SC12B_H__
#define __INT_SC12B_H__

#include "Dri_i2c.h"

#define IIC_SDA_SEL (1ULL << IIC_SDA_PIN)
#define IIC_SCL_SEL (1ULL << IIC_SCL_PIN)
#define SC12B_INT_SEL (1ULL << GPIO_NUM_0)

/// SC12B的中断引脚P0
#define SC12B_INT_PIN GPIO_NUM_0

//ASEL 引脚为低电平，设备的地址
#define SC12B_ADDR 0x42 

void Int_SC12B_Init(void);

uint16_t Int_SC12B_SimpleRead(void);

int8_t Int_SC12B_GetKeyValue(void);
#endif /* __INT_SC12B_H__ */
