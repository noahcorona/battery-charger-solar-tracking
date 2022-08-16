#include "stm32l476xx.h"
#include <stdio.h>
#include "SysClock.h"
#include "I2C.h"
#include "PWM.h"
#include "sensors.h"

#define LCD_BLINK_CMD 0x80       	// I2C register for BLINK setting
#define LCD_BLINK_DISPLAYON 0x01 	// I2C value for steady on
#define LCD_BLINK_OFF 0          	// I2C value for steady off
#define LCD_CMD_BRIGHTNESS 0xE0 	// I2C register for BRIGHTNESS setting
#define LCD_BRIGHTNESS_MAX 15 		// I2C register for BRIGHTNESS setting

extern uint16_t batteryLoopSec;
extern uint16_t controlLoopMs;

uint16_t sensorValues[4];
volatile uint16_t batteryCharge = 0;
volatile uint8_t count = 0;

/*
 * Peripheral test functions
 */

void testLCD() {
	writeToDisplay("100%");
}


void testMotors(uint8_t minH, uint8_t maxH, uint8_t minV, uint8_t maxV) {
	// goes once around perimeter of defined boundaries
	delay(25);
	setAngle(1, (minH + maxH) / 2);
	delay(25);
	setAngle(2, (minV + maxV) / 2);
	
	for(uint8_t i = (minH + maxH) / 2; i < maxH; ++i) {
		delay(25);
		setAngle(1, i);
	}
	
	for(uint8_t i = (minV + maxV) / 2; i < maxV; ++i) {
		delay(25);
		setAngle(2, i);
	}
	
	for(int16_t i = maxH - 1; i > minH; --i) {
		delay(25);
		setAngle(1, i);
	}
	
	for(int16_t i = maxV - 1; i > minV; --i) {
		delay(25);
		setAngle(2, i);
	}
	
	for(int16_t i = minH; i <= (minH + maxH) / 2; ++i) {
		delay(25);
		setAngle(1, i);
	}
	
	for(int16_t i = minV; i <= (minV + maxV) / 2; ++i) {
		delay(25);
		setAngle(2, i);
	}
	
}


void testLightSensor() {
	// address of all 4 light sensors
	uint8_t SlaveAddress = 0x23 << 1;
	uint8_t readCmd = 0x11;
	
	// select port 0 - 3 for 4 sensors
	uint8_t port = 1;
	
	uint16_t val  = readWord(I2C1, SlaveAddress, port, readCmd);
	
}


/*
 * Light sensor functions
 */

void readLuxValues() {	
	// 4 I2C light sensors to read from, 
	// all with same address on ports 0 to 3
	// and read_cmd = 0x11
	for(uint8_t port = 0; port < 4; port++)
		sensorValues[port] = readWord(I2C1, 0x23 << 1, port, 0x11);
}


/*
 * Servo movement function
 *
 * simple algorithm to move towards a britghter light source
 * based on a tolerance (threshold in the difference in top/bottom
 * and left/right)
 */

void moveTowardLight(int32_t tolerance) {
	
	// calculate left/right difference
		int32_t diffH = ((sensorValues[0] + sensorValues[2]) / 2) - ((sensorValues[1] + sensorValues[3]) / 2);
		
		// move left/right if needed
		if(diffH > tolerance) {
			decrementAngle(1);
		} else if(diffH < -tolerance) {
			incrementAngle(1);
		}
		
		// calculate top/bottom difference
		int32_t diffV = ((sensorValues[0] + sensorValues[1]) / 2) - ((sensorValues[2] + sensorValues[3]) / 2);
		
		// move up/down if needed
		if(diffV > tolerance) {
			incrementAngle(2);
		} else if(diffV < -tolerance) {
			decrementAngle(2);
		}

}


/*
 * LCD functions
 */

void LCD_Init() {

	// first send a command multiplexer to tell it the port
	// to send data to (port 4 for our display)
	uint8_t MultiplexerAddress = 0x70 << 1;
	uint8_t port = 4;
	uint8_t Data_Send = (uint8_t) (1U << port);
	
	I2C_SendData(I2C1, MultiplexerAddress, &Data_Send, 1);
	
	uint8_t SlaveAddress = 0x71 << 1;	
	
	// write 0x21 to turn on oscillator
	Data_Send = 0x21;
	I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
	
	// set up blink setting
	Data_Send = LCD_BLINK_CMD | LCD_BLINK_DISPLAYON | LCD_BLINK_OFF;
	I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
	
	// set up brightness
	Data_Send = LCD_CMD_BRIGHTNESS | LCD_BRIGHTNESS_MAX;
	I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
}

void writeToDisplay(char* str) {	
	// first send a command multiplexer to tell it the port
	// to send data to (port 4 for our display)
	uint8_t MultiplexerAddress = 0x70 << 1;
	uint8_t port = 4;
	uint8_t Data_Send[9];
	Data_Send[0] = (uint8_t) (1U << port);
	I2C_SendData(I2C1, MultiplexerAddress, Data_Send, 1);
	
	uint8_t SlaveAddress = 0x71 << 1;
	
	// clear display
	for(uint8_t i = 0; i < 9; i += 1)
		Data_Send[i] = 0;
	
	I2C_SendData(I2C1, SlaveAddress, Data_Send, 9);
	
	uint8_t j = 0;
	for(uint8_t i = 1; i < 8; i += 2) {
		switch(str[j]) {
			case '1':
				// data = 0x0006;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0x06;
			break;
			case '2':
				// data = 0x00DB;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0xDB;
			break;
			case '3':
				// data = 0x00CF;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0xCF;
			break;
			case '4':
				// data = 0x00E6;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0xE6;
			break;
			case '5':
				// data = 0x00ED;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0xED;
			break;
			case '6':
				// data = 0x00FD;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0xFD;
			break;
			case '7':
				// data = 0x0007;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0x07;
			break;
			case '8':
				// data = 0x00FF;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0xFF;
			break;
			case '9':
				//data = 0x00E7;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0xE7;
			break;
			case '0':
				// data = 0x003F;
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0x3F;
			break;
			case '%':
				//data = 0x0C24;
				Data_Send[i + 1] = 0x0C;
				Data_Send[i] = 0x24;
			break;
			default:
				Data_Send[i + 1] = 0x00;
				Data_Send[i] = 0x00;
			break;
		}
		
		j += 1;
	}
	
	I2C_SendData(I2C1, SlaveAddress, Data_Send, 9);
			
}

void displayCharge() {
	// convert most recent charge to string
	char str[5];
	sprintf(str, "%u%%", batteryCharge);
	
	// update battery display via I2C
	writeToDisplay(str);
}



/*
 * "Fuel guage" functions
 */

void Battery_Monitor_Init(uint32_t updateLoopSec) {
	batteryLoopSec = updateLoopSec;
}


void readBatteryCharge() {
	// write "quick start" command to mode reg
	writeWord(I2C2, 0x32 << 1, 0, 0x06, 0x0040);
	for(uint32_t i = 0; i < 500000; i += 1) { };
	batteryCharge = (readWord(I2C2, 0x32 << 1, 0, 0x04) >> 8)/1.6;
}

 
/*
 * Timing-based loops
 */

void controlLoop(uint32_t currentMs) {
		if(currentMs == 1)
			readLuxValues();
		else if(currentMs == 100) {
			moveTowardLight(5);
			count++;
		}
		
		if(count == batteryLoopSec * 5) {
			readBatteryCharge();
			displayCharge();
			count = 0;
	}
}
