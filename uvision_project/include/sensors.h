#ifndef __STM32L476G_DISCOVERY_SENSORS_H
#define __STM32L476G_DISCOVERY_SENSORS_H

#include "stm32l476xx.h"
#include <stdio.h>
#include "I2C.h"
#include "PWM.h"

#define HT16K33_BLINK_CMD 0x80       ///< I2C register for BLINK setting
#define HT16K33_BLINK_DISPLAYON 0x01 ///< I2C value for steady on
#define HT16K33_BLINK_OFF 0          ///< I2C value for steady off
#define HT16K33_CMD_BRIGHTNESS 0xE0 ///< I2C register for BRIGHTNESS setting
#define HT16K33_BRIGHTNESS_MAX 12 ///< I2C register for BRIGHTNESS setting

extern uint16_t sensorValues[4];
volatile extern uint16_t batteryCharge;

void testMotors(uint8_t minH, uint8_t maxH, uint8_t minV, uint8_t maxV);
void testLightSensor(void);
void testLCD(void);

// LCD functions
void LCD_Init(void);
void writeToDisplay(char* str);
void displayCharge(void);

// MAX17043 functions
void Battery_Monitor_Init(uint32_t batteryLoopMs);
void readBatteryCharge(void);

// BH1750 and servo helper function
void readLuxValues(void);
void moveTowardLight(int32_t tolerance);

void controlLoop(uint32_t currentMs);
void batteryLoop(uint32_t currentMs);

#endif
