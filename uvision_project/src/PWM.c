/*
	
	PWM is used for the two MG995 servos that
	control the pan and tilt brackets
	
	All of the timing values used are found
	in the timing_calculations spreadsheet
	and GPIO values used are found in the
	GPIO_pins spreadsheet rather than putting
	them scattered in comments
	
*/

#include "stm32l476xx.h"
#include "PWM.h"
#include "SysClock.h"

#define MOTOR_1 1
#define MOTOR_2 2

uint32_t currentAngle1;
uint32_t currentAngle2;

extern uint8_t maxDegH;
extern uint8_t minDegH;

extern uint8_t maxDegV;
extern uint8_t minDegV;

// initialize GPIO pins A8 (motor 1) and A5 (motor 2)
void PWM_GPIO_Init(void) {
	// start GPIO port A clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// Configure GPIO pin A8 for TIM1 channel 1
	GPIOA->MODER &= ~GPIO_MODER_MODE8;				// Configure port for alt. func (10)
	GPIOA->MODER |= GPIO_MODER_MODE8_1;
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8; 			// and set AF value to TIM1_CH1 (0001)
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL8_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8; 			// set PUPDR to no PU/no PD (00)
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8; 			// set output type to push-pull (0)
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8; 	// set speed to very high (11)
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;				// enable clock of TIM1
	
	// Configure GPIO pin A0 for TIM2 channcel 1
	GPIOA->MODER &= ~GPIO_MODER_MODE5;				// Configure port for alt. func (10)
	GPIOA->MODER |= GPIO_MODER_MODE5_1;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5; 			// and set AF value to TIM2_CH1 (0001)
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5; 				// set PUPDR to no PU/no PD (00)
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5; 				// set output type to push-pull (0)
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5; 	// set speed to very high (11)
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;			// enable clock of TIM2
}

// initialize PWM timing
void PWM_Init(void) {
	
	currentAngle1 = 90;
	currentAngle2 = 135;
	
	PWM_GPIO_Init();
	
	// Configure TIM1 channel 1 for PWM OUT (motor 1)
	TIM1->PSC = 79;
	TIM1->ARR = 19999;
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M; 		// configure TIM1 mode to be output compare ch 1
	TIM1->CCMR1 |= (TIM_CCMR1_OC1M_1  	// output compare = '0110'
								| TIM_CCMR1_OC1M_2
								| TIM_CCMR1_OC1PE);		// enable preload
	TIM1->CCER &= ~TIM_CCER_CC1P; 			// CCxP = 1 -> active high, 0 -> active low
	TIM1->CCER |= TIM_CCER_CC1E; 				// enable channel 1 output
	TIM1->BDTR |= TIM_BDTR_MOE; 				// enable main output
	TIM1->CCR1 = 
			getCCRForAngle(currentAngle1); 	// set default capture/compare value
	TIM1->CR1 |= TIM_CR1_DIR 						// enable the counter and set dir
							| TIM_CR1_CEN; 
	
	// Configure TIM2 channel 1 for PWM OUT (motor 2)
	TIM2->PSC = 79;
	TIM2->ARR = 19999;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; 		// configure TIM1 mode to be output compare ch 1
	TIM2->CCMR1 |= (TIM_CCMR1_OC1M_1  	// output compare = '0110'
								| TIM_CCMR1_OC1M_2
								| TIM_CCMR1_OC1PE); 	// enable preload
	TIM2->CCER &= ~TIM_CCER_CC1P; 			// CCxP = 1 -> active high, 0 -> active low
	TIM2->CCER |= TIM_CCER_CC1E; 				// enable channel 1 output
	TIM2->BDTR |= TIM_BDTR_MOE; 				// enable main output
	TIM2->CCR1 = 
			getCCRForAngle(currentAngle2); 	// set default capture/compare value
	TIM2->CR1 |= TIM_CR1_DIR 						// enable the counter and set dir
							| TIM_CR1_CEN; 
}

// get the CCR value corresponding to the desired angle (0 to 180 deg)
uint32_t getCCRForAngle(uint32_t degrees) {
	return (uint32_t) (((double) degrees / 180.0) * (2000) + 500);
}

// increment the angle by 1 for desired motor
// if at the maximum angle, rotates to minimum angle
void incrementAngle(uint32_t motorNum) {
	uint32_t currentAngle = 0;
	
	if (motorNum == MOTOR_1 && currentAngle1 < maxDegH)
		currentAngle = currentAngle1;
	else if (motorNum == MOTOR_2 && currentAngle2 < maxDegV)
		currentAngle = currentAngle2;
	else 
		return;

	currentAngle += 1;
	
	setAngle(motorNum, currentAngle);
}

// decrement the angle by 1 for desired motors
// if at the minimum angle, rotates to maximum angle
void decrementAngle(uint32_t motorNum) {
	uint32_t currentAngle = 0;
	
	if (motorNum == MOTOR_1 && currentAngle1 > minDegH)
		currentAngle = currentAngle1;
	else if (motorNum == MOTOR_2 && currentAngle2 > minDegV)
		currentAngle = currentAngle2;
	else 
		return;
	
	currentAngle -= 1;
	
	setAngle(motorNum, currentAngle);
}

// returns angle of the desired motors
uint32_t getAngle(uint32_t motorNum) {
	if(motorNum == 1) {
		return currentAngle1;
	} else {
		return currentAngle2;
	}
}

// set angle of motor 1 or 2 to the given angle
void setAngle(uint32_t motorNum, uint32_t newAngle) {
		uint32_t ccrVal = getCCRForAngle(newAngle);
		if (motorNum == 1) {
			TIM1->CCR1 = ccrVal;
			currentAngle1 = newAngle;
		} else {
			TIM2->CCR1 = ccrVal;
			currentAngle2 = newAngle;
	}
}
