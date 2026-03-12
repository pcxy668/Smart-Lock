#ifndef __INT_FPM383_H__
#define __INT_FPM383_H__

#include "driver/gpio.h"
#include "driver/uart.h"
#include "Com_utils.h"

#define FPM383_INT_PIN GPIO_NUM_10

void Int_FPM383_Init(void);
void Int_FPM383_GetSerialNumber(void);
uint8_t Int_FPM383_Sleep(void);
uint8_t Int_FPM383_GetTemplatesNumber(void);
uint8_t Int_FPM383_GetImage(void);
uint8_t Int_FPM383_GenChar(uint8_t bufferID);
uint8_t Int_FPM383_Search(void);
uint8_t Int_FPM383_RegModel(void);
uint8_t Int_FPM383_StoreChar(uint8_t pageID);
uint8_t Int_FPM383_WriteReg(uint8_t regNum,uint8_t regValue);
uint8_t Int_FPM383_Enroll(void);
uint8_t Int_FPM383_Identify(void);

#endif /* __INT_FPM383_H__ */
