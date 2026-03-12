#ifndef __INT_MOTOR_H__
#define __INT_MOTOR_H__

#include <stdint.h>
#include "driver/gpio.h"
#include "Com_utils.h"

#define MOTOR_IN1_PIN GPIO_NUM_4
#define MOTOR_IN2_PIN GPIO_NUM_5

void Int_Motor_Init(void);
void Int_Motor_OpenLock(void);

#endif /* __INT_MOTOR_H__ */
