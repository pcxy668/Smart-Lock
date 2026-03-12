#ifndef __DRI_I2C_H__
#define __DRI_I2C_H__

#include <stdint.h>
#include "driver/gpio.h"
#include "Com_utils.h"

// SCL引脚P1
#define IIC_SCL_PIN GPIO_NUM_1
// SDA引脚P2
#define IIC_SDA_PIN GPIO_NUM_2

#define IIC_SDA_HIGH gpio_set_level(IIC_SDA_PIN, 1)
#define IIC_SDA_LOW  gpio_set_level(IIC_SDA_PIN, 0)

#define IIC_SCL_HIGH gpio_set_level(IIC_SCL_PIN, 1)
#define IIC_SCL_LOW  gpio_set_level(IIC_SCL_PIN, 0)

#define IIC_SDA_READ gpio_get_level(IIC_SDA_PIN)

#define IIC_SDA_OUT gpio_set_direction(IIC_SDA_PIN, GPIO_MODE_OUTPUT)
#define IIC_SDA_IN  gpio_set_direction(IIC_SDA_PIN, GPIO_MODE_INPUT)

void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_SendByteAndGetNACK(uint8_t byte);
uint8_t IIC_ReceiveByte(void);
void IIC_Respond(uint8_t ACKSignal);

#endif /* __DRI_I2C_H__ */
