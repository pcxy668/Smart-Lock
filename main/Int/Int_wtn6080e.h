#ifndef __INT_WTN6080E_H__
#define __INT_WTN6080E_H__

#include <stdint.h>
#include "driver/gpio.h"
#include "Com_utils.h"

#define WTN6080E_DATA_PIN GPIO_NUM_7
#define WTN6080E_DATA_SEL (1ULL << WTN6080E_DATA_PIN)

void Int_WTN6080E_Init(void);
void Int_WTN6080E_Play(uint8_t byte);

#endif /* __INT_WTN6080E_H__ */
