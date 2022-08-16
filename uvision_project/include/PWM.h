#ifndef __STM32L476G_DISCOVERY_PWM_H
#define __STM32L476G_DISCOVERY_PWM_H

#include "stm32l476xx.h"

uint32_t getAngle(uint32_t motorNum);
void PWM_GPIO_Init(void);
void PWM_Init(void);
uint32_t getCCRForAngle(uint32_t angle);
void incrementAngle(uint32_t motorNum);
void decrementAngle(uint32_t motorNum);
void setAngle(uint32_t motorNum, uint32_t newAngle);

#endif
