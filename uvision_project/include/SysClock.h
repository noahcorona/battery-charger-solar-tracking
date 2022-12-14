#ifndef __STM32L476G_DISCOVERY_CLOCK_H
#define __STM32L476G_DISCOVERY_CLOCK_H

#include "stm32l476xx.h"

void System_Clock_Init();
void SysTick_Init();
void SysTick_Handler(void);
void delay(uint32_t T);
#endif
