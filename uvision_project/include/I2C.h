#ifndef __STM32L476G_DISCOVERY_I2C_H
#define __STM32L476G_DISCOVERY_I2C_H

#include <stdint.h>
#include "stm32l476xx.h"

#define READ_FROM_SLAVE 1
#define WRITE_TO_SLAVE  0

void I2C1_GPIO_Init(void);
void I2C2_GPIO_Init(void);
void I2C1_Init(void);
void I2C2_Init(void);

int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction);
void I2C_Stop(I2C_TypeDef * I2Cx);
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx);
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size);
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size);
uint16_t readWord(I2C_TypeDef * I2Cx, uint8_t sAddr, uint8_t port, uint8_t cmd);
void writeWord(I2C_TypeDef * I2Cx, uint8_t sAddr, uint8_t port, uint8_t cmd, uint16_t val);
void CODEC_Initialization(void);


#endif
