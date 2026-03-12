#include "Dri_i2c.h"

void IIC_Start(void) {
  IIC_SDA_OUT;
  IIC_SDA_HIGH;
  IIC_SCL_HIGH;
  DelayMs(1);
  IIC_SDA_LOW;
  DelayMs(1);
  IIC_SCL_LOW;
  DelayMs(1);
}

void IIC_Stop(void) {
  IIC_SCL_LOW;
  IIC_SDA_OUT;
  IIC_SDA_LOW;
  DelayMs(1);
  IIC_SCL_HIGH;
  DelayMs(1);
  IIC_SDA_HIGH;
}

// 发送一个字节，并接受应答
uint8_t IIC_SendByteAndGetNACK(uint8_t byte) {
  uint8_t i = 0;
  IIC_SDA_OUT;
  for (i = 0; i < 8; i++) {
    IIC_SCL_LOW;
    DelayMs(1);
    if ((byte >> 7) & 0x01) {
      IIC_SDA_HIGH;
    } else {
      IIC_SDA_LOW;
    }
    DelayMs(1);
    IIC_SCL_HIGH;
    DelayMs(1);
    byte <<= 1;
  }

  IIC_SCL_LOW;
  DelayMs(3);
  IIC_SDA_IN;
  DelayMs(3);

  IIC_SCL_HIGH;
  DelayMs(1);
  i = 250;
  while (i--) {
    if (!IIC_SDA_READ) {
      IIC_SCL_LOW;
      return 0;
    }
  }

  IIC_SCL_LOW;
  return 1;
}

// 读取一个字节
uint8_t IIC_ReceiveByte(void) {
  uint8_t i = 0;
  uint8_t buffer = 0;
  IIC_SDA_IN;
  IIC_SCL_LOW;
  for (i = 0; i < 8; i++) {
    DelayMs(1);
    IIC_SCL_HIGH;
    buffer = (buffer << 1) | IIC_SDA_READ;
    DelayMs(1);
    IIC_SCL_LOW;
  }

  return buffer;
}

// 下发应答命令
void IIC_Respond(uint8_t ACKSignal) {
  IIC_SDA_OUT;
  IIC_SDA_LOW;
  IIC_SCL_LOW;
  if (ACKSignal) {
    IIC_SDA_HIGH;
  } else {
    IIC_SDA_LOW;
  }
  DelayMs(1);
  IIC_SCL_HIGH;
  DelayMs(1);
  IIC_SCL_LOW;
}
