#include "I2C.h"
#include "SysClock.h"
#include <stddef.h>

extern void Error_Handler(void);
	
//===============================================================================
//                        I2C GPIO Initialization
//===============================================================================
void I2C1_GPIO_Init(void) {
	// enable GPIO port B clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	// AFR value of 4 for i2c on pb6/pb7
	// OSPEED = very high (11) - reset is 00
	// OTYPE = open drain (1) - reset is 0
	// PUPDR = pull up (01) - reset is 00
	// MODER = alf func (10) - reset is 11
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL7_2;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);

}

void I2C2_GPIO_Init(void) {
	// enable GPIO port B clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	// AFR value of 4 for i2c on pb10/pb11
	// OSPEED = very high (11) - reset is 00
	// OTYPE = open drain (1) - reset is 0
	// PUPDR = pull up (01) - reset is 00
	// MODER = alf func (10) - reset is 11
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL10_2 | GPIO_AFRH_AFSEL11_2;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED10 | GPIO_OSPEEDR_OSPEED11;
	GPIOB->OTYPER |= GPIO_OTYPER_OT10 | GPIO_OTYPER_OT11;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD10_0 | GPIO_PUPDR_PUPD11_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODE10_0 | GPIO_MODER_MODE11_0);
}
//===============================================================================
//                          I2C Initialization
//===============================================================================

void I2C1_Init(void){
	I2C1_GPIO_Init();
	
	uint32_t OwnAddr = 0x52;
	
	I2C1->CR1 &= ~I2C_CR1_PE;
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	RCC->CCIPR |= RCC_CCIPR_I2C1SEL_0;

	RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;
	
	I2C1->CR1 |= I2C_CR1_ERRIE;
	I2C1->CR2 |= I2C_CR2_AUTOEND;
	
	// 		 - presc = 9 (clock prescalar) 
	//					f_i2cclk = f_sysclk = 80MHz
	//					f_presc = f_i2cclk / (presc + 1) = 8 MHz
	//					t_i2cclk = 1.25*10^-8 sec = 12.5 ns
	//					t_presc = (presc + 1) * t_i2cclk = 1.25*10^-7 sec = 125 ns
	// 		 - scldel (setup time, delay between SDA edge and next scl rising edge)
	// 					TC74 specifies t_setup > 1000 ns = 1.0 us (p. 1105 ref man)
	//				  t_scldel = (scldel + 1) * t_presc = 1.0 us
	//					scldel = (t_scldel/t_presc) - 1 = 7
	// 		 - sdadel (hold time, delay between scl falling edge and next sda rising edge)
	// 					TC74 specifies t_hold > 1250 ns = 1.25 us
	//				  t_sdadel = (sdadel + 1) * t_presc = 1.25 us
	//					sdadel = (t_sdadel/t_presc) - 1 = 9
	// 		 - sclh (serial clock high period)
	// 					TC74 specifies t_high_min = 4.0 us
	//					t_sclh = (sclh + 1) * t_pres = 4.0 us
	//					sclh = (t_sdadel/t_presc) - 1 = 31
	// 		 - scll (serial clock low period)
	// 					TC74 specifies t_low_min = 4.7 us
	//					t_scll = (scll + 1) * t_presc >= 4.7 us = 4.75 us
	//					scll = (t_scll/t_presc) - 1 = 36.6 -> 37
	I2C1->TIMINGR |= (0x9 << 28) | (0x7 << 20) | (0x9 << 16) | (0x1F << 8) | 0x25;
	
	
	I2C1->OAR1 &= ~I2C_OAR1_OA1EN;
	I2C1->OAR1 |= OwnAddr;
	I2C1->OAR1 |= I2C_OAR1_OA1EN;
	
	I2C1->CR1 |= I2C_CR1_PE;
}

void I2C2_Init(void){
	I2C2_GPIO_Init();
	
	uint32_t OwnAddr = 0x52;
	
	I2C2->CR1 &= ~I2C_CR1_PE;

	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C2EN;
	RCC->CCIPR |= RCC_CCIPR_I2C2SEL_0;
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C2RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C2RST;
	
	I2C2->CR1 |= I2C_CR1_ERRIE;
	I2C2->CR2 |= I2C_CR2_AUTOEND;

	// 		 - presc = 9 (clock prescalar) 
	//					f_i2cclk = f_sysclk = 80MHz
	//					f_presc = f_i2cclk / (presc + 1) = 8 MHz
	//					t_i2cclk = 1.25*10^-8 sec = 12.5 ns
	//					t_presc = (presc + 1) * t_i2cclk = 1.25*10^-7 sec = 125 ns
	// 		 - scldel (setup time, delay between SDA edge and next scl rising edge)
	// 					MAX7043 specifies t_setup > 100 ns = 0.1 us (p. 1105 ref man)
	//				  t_scldel = (scldel + 1) * t_presc = 0.1 us
	//					scldel = (t_scldel/t_presc) - 1 = 2
	// 		 - sdadel (hold time, delay between scl falling edge and next sda rising edge)
	// 					MAX7043 specifies t_hold < 900 ns = 0.9 us
	//				  t_sdadel = (sdadel + 1) * t_presc = 0.9 us
	//					sdadel = (t_sdadel/t_presc) - 1 = 6.2 -> 6
	// 		 - sclh (serial clock high period)
	// 					MAX7043 specifies t_high > 0.6 us
	//					t_sclh = (sclh + 1) * t_pres = 0.6 us
	//					sclh = (t_sdadel/t_presc) - 1 = 3.8 -> 4
	// 		 - scll (serial clock low period)
	// 					MAX7043 specifies t_low_min = 1.3 us
	//					t_scll = (scll + 1) * t_presc >= 1.3 us
	//					scll = (t_scll/t_presc) - 1 = 9.4 -> 10
	I2C2->TIMINGR |= (0x9 << 28) | (2UL << 20) | (6UL << 16) | (4UL << 8) | 10UL;
	
	I2C2->OAR1 &= ~I2C_OAR1_OA1EN;
	I2C2->OAR1 |= OwnAddr;
	I2C2->OAR1 |= I2C_OAR1_OA1EN;
	
	I2C2->CR1 |= I2C_CR1_PE;
}

//===============================================================================
//                           I2C Start
// Master generates START condition:
//    -- Slave address: 7 bits
//    -- Automatically generate a STOP condition after all bytes have been transmitted 
// Direction = 0: Master requests a write transfer
// Direction = 1: Master requests a read transfer
//=============================================================================== 
int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction) {	
	
	// Direction = 0: Master requests a write transfer
	// Direction = 1: Master requests a read transfer
	
	uint32_t tmpreg = 0;
	
	// This bit is set by software, and cleared by hardware after the Start followed by the address
	// sequence is sent, by an arbitration loss, by a timeout error detection, or when PE = 0.
	tmpreg = I2Cx->CR2;
	
	tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));
	
	if (Direction == READ_FROM_SLAVE)
		tmpreg |= I2C_CR2_RD_WRN;  // Read from Slave
	else
		tmpreg &= ~I2C_CR2_RD_WRN; // Write to Slave
		
	tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES));
	
	tmpreg |= I2C_CR2_START;
	
	I2Cx->CR2 = tmpreg; 
	
   	return 0;  // Success
}

//===============================================================================
//                           I2C Stop
//=============================================================================== 
void I2C_Stop(I2C_TypeDef * I2Cx){
	// Master: Generate STOP bit after the current byte has been transferred 
	I2Cx->CR2 |= I2C_CR2_STOP;								
	// Wait until STOPF flag is reset
	while( (I2Cx->ISR & I2C_ISR_STOPF) == 0 ); 
}

//===============================================================================
//                           Wait for the bus is ready
//=============================================================================== 
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx){
	// Wait until I2C bus is ready
	while( (I2Cx->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY );	// If busy, wait
}

//===============================================================================
//                           I2C Send Data
//=============================================================================== 
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if (Size <= 0 || pData == NULL) return -1;
	
	I2C_WaitLineIdle(I2Cx);
	
	if (I2C_Start(I2Cx, DeviceAddress, Size, WRITE_TO_SLAVE) < 0 ) return -1;

	// Send Data
	// Write the first data in DR register
	// while((I2Cx->ISR & I2C_ISR_TXE) == 0);
	// I2Cx->TXDR = pData[0] & I2C_TXDR_TXDATA;  

	for (i = 0; i < Size; i++) {
		// TXE is set by hardware when the I2C_TXDR register is empty. It is cleared when the next
		// data to be sent is written in the I2C_TXDR register.
		// while( (I2Cx->ISR & I2C_ISR_TXE) == 0 ); 

		// TXIS bit is set by hardware when the I2C_TXDR register is empty and the data to be
		// transmitted must be written in the I2C_TXDR register. It is cleared when the next data to be
		// sent is written in the I2C_TXDR register.
		// The TXIS flag is not set when a NACK is received.
		
		while((I2Cx->ISR & I2C_ISR_TXIS) == 0 );
		I2Cx->TXDR = pData[i] & I2C_TXDR_TXDATA;  // TXE is cleared by writing to the TXDR register.
		
	}
	
	// Wait until TC flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0 && (I2Cx->ISR & I2C_ISR_NACKF) == 0);
	
	
	if( (I2Cx->ISR & I2C_ISR_NACKF) != 0 ) return -1;

	I2C_Stop(I2Cx);
	return 0;
}


//===============================================================================
//                           I2C Receive Data
//=============================================================================== 
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if(Size <= 0 || pData == NULL) return -1;

	I2C_WaitLineIdle(I2Cx);

	I2C_Start(I2Cx, DeviceAddress, Size, READ_FROM_SLAVE); // 0 = sending data to the slave, 1 = receiving data from the slave
						  	
	for (i = 0; i < Size; i++) {
		// Wait until RXNE flag is set 	
		while( (I2Cx->ISR & I2C_ISR_RXNE) == 0 );
		pData[i] = I2Cx->RXDR & I2C_RXDR_RXDATA;
	}
	
	// Wait until TCR flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0);
	
	I2C_Stop(I2Cx);
	
	return 0;
}

//===============================================================================
//                         16-bit helper functions
//=============================================================================== 

uint16_t readWord(I2C_TypeDef * I2Cx, uint8_t sAddr, uint8_t port, uint8_t cmd) {
	uint8_t Data[2] = { 0x0, 0x0 };
	
	if(I2Cx == I2C1) {
		// set correct port (I2C1 is 1-to-8 multiplexer, ports 0-7)
		uint8_t mData = (uint8_t) (1U << port);
		
		// first send a command multiplexer to tell it the port
		// to send data to (ports 0-3 for our sensors)
		I2C_SendData(I2C1, 0x70 << 1, &mData, 1);

	}
	
	// send desired command/mem addr. to slave
	I2C_SendData(I2Cx, sAddr, &cmd, 1);
		
	// next request 2 bits from slave
	I2C_ReceiveData(I2Cx, sAddr, Data, 2);

  return ((uint16_t) Data[0] << 8U) | ((uint16_t) Data[1]);
}

void writeWord(I2C_TypeDef * I2Cx, uint8_t sAddr, uint8_t port, uint8_t cmd, uint16_t val) {
	uint8_t Data_Send[3] = {cmd, val & 0xFF, (val >> 8) & 0xFF};
	
	if(I2Cx == I2C1) {
		uint8_t* mData_Send = &cmd;
		
		// set correct port (I2C1 is 1-to-8 multiplexer, ports 0-7)
		*mData_Send = (uint8_t) (1U << port);
		
		// first send a command multiplexer to tell it the port
		// to send data to (ports 0-3 for our sensors)
		I2C_SendData(I2C1, 0x70 << 1, mData_Send, 1);
	}
	
	// send desired command/mem addr. to slave
	I2C_SendData(I2Cx, sAddr, Data_Send, 3);
}
