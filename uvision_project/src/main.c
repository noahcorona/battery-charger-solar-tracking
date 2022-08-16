/*
 * ECE 153B - Winter 2020-21
 *
 * Name(s): Noah Corona
 * Section: Tuesday 7:00 PM
 * Final project: Solar tracker
 */
 
#include "stm32l476xx.h"
#include <stdio.h>
#include "SysClock.h"
#include "PWM.h"
#include "I2C.h"
#include "sensors.h"

uint16_t controlLoopMs = 100;		// defines the control loop speed
uint32_t batteryLoopSec = 1;		// defines the battery read speed

uint8_t maxDegH = 140;					// horizontal rotation servo max angle (degrees)
uint8_t minDegH = 40;						// horizontal rotation servo max angle (degrees)

uint8_t maxDegV = 170;					// vertical rotation servo max angle (degrees)
uint8_t minDegV = 90;						// vertical rotation servo max angle (degrees)

uint8_t start = 0;							// start flag for control and battery loops

uint16_t vals[255] = {0};
uint16_t val = 0;
uint8_t cmd = 0;

int main(void) {
	System_Clock_Init();
	
	PWM_Init(); 
	I2C1_Init();
	I2C2_Init();
	LCD_Init();
	Battery_Monitor_Init(batteryLoopSec);

	start = 1;
	
	while(1);

}
