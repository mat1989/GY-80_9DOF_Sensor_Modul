/*
 * twi.c
 *
 *  Created on: 24.09.2015
 *      Author: matthias
 */

#include <avr/io.h>
//#include <util/delay.h>
#include <stdio.h>
#include <util/twi.h>
#include "twi.h"
#include "uart.h"

/*
 * Low Level Funktionen
 * *************************************************************
 */
uint8_t calcAdr(uint8_t adr, uint8_t modus){
	uint8_t i2cAdr = (adr << 1) | modus;
	return i2cAdr;
}

//Quelle: http://www.embedds.com/programming-avr-i2c-interface/
void write(uint8_t u8data) {
	TWDR = u8data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0) {
	}
}

uint8_t readACK(void) {	//uint8_t
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while ((TWCR & (1 << TWINT)) == 0) {
	}
	return TWDR;
}

//read byte with NACK
uint8_t readNACK(void) { //uint8_t
	TWCR = (1 << TWINT) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0) {
	}
	return TWDR;
}

uint8_t getStatus(void) {
	uint8_t status;
	//mask status
	status = TWSR & 0xF8;
	return status;
}

/*
 * Mid Level
 * *************************************************************
 */
void TWI_Init(void) {
	//set SCL to 400kHz
//	TWSR = 0x00;
//	TWBR = 0x0C;
	TWBR = ~((1 << TWPS0) | (1 << TWPS1)); 	//Prescaler
	TWSR = 32;								// 8MHz 100kHz
	//enable TWI
	TWCR = (1 << TWEN);
}

char TWI_Start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0) {
	}
	//_delay_ms(5);
	char twi_status = getStatus();
	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) {
		//Error
		return 0;
	} else {
		return 1;
	}
}

void TWI_Stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	//_delay_ms(5);
}

char TWI_Write_Addr(uint8_t i2cAdr, uint8_t i2cModus) {
	uint8_t adr = calcAdr(i2cAdr, i2cModus);
	write(adr);
	//_delay_ms(5);
	char twi_status = getStatus();
	if ((twi_status != TW_MR_SLA_ACK) && (twi_status != TW_MT_SLA_ACK)) {
		//Error
		return 0;
	} else {
		return 1;
	}
}

char TWI_Write_Func(uint8_t u8data) {
	write(u8data);
	//_delay_ms(5);
	if (getStatus() != TW_MT_DATA_ACK) {
		//Error
		return 0;
	} else {
		return 1;
	}
}

char TWI_Read(uint8_t reply[], uint8_t n_Byte) {
	//TWI Read Data
	for (unsigned char i = 0; i < n_Byte; i++) {
		reply[i] = readACK();
//		uart_writeInt8(reply[i]);
//		uart_writeString("-");
		//_delay_ms(5);
		if (getStatus() != TW_MR_DATA_ACK) {
			//Error
			return 0;
		}
	}
	reply[n_Byte] = readNACK();
//	uart_writeInt8(reply[n_Byte]);
//	uart_writeAbsatz();
	if (getStatus() != TW_MR_DATA_NACK) {
		//Error
		return 0;
	} else {
		return 1;
	}
}

/*
 * High Level
 * *************************************************************
 */
void TWI_writeRegister(uint8_t i2cAdr, uint8_t regAdr, uint8_t val) {
	if (TWI_Start() == 0) {
		//Error
		uart_writeString("TWI Error Start");
		uart_writeAbsatz();
	}
	if (TWI_Write_Addr(i2cAdr, TW_WRITE) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Write_Func(regAdr) == 0) {
		//Error
		uart_writeString("TWI Error regAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Write_Func(val) == 0) {
		//Error
		uart_writeString("TWI Error val Write");
		uart_writeAbsatz();
	}
	TWI_Stop();
}
/*
 * Liest "1" Register
 */
char TWI_readRegister(uint8_t i2cAdr, uint8_t regAdr) {
	uint8_t reply[3];
	uint8_t n_Byte = 1;

	if (TWI_Start() == 0) {
		//Error
		uart_writeString("TWI Error Start");
		uart_writeAbsatz();
	}
	if (TWI_Write_Addr(i2cAdr, TW_WRITE) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Write_Func(regAdr) == 0) {
		//Error
		uart_writeString("TWI Error regAdr Write");
		uart_writeAbsatz();
	}
	TWI_Stop();

	if (TWI_Start() == 0) {
		//Error
		uart_writeString("TWI Error Start");
		uart_writeAbsatz();
	}
	if (TWI_Write_Addr(i2cAdr, TW_READ) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Read");
		uart_writeAbsatz();
	}
	if (TWI_Read(reply, n_Byte) == 0) {
		//Error
		uart_writeString("TWI Error Read Byte");
		uart_writeAbsatz();
	}
	TWI_Stop();
	return reply[0];
}

/*
 * Liest "2" Register (aufeinander folgende Register e.g.: 0x01, 0x02)
 */
short TWI_readRegister2(uint8_t i2cAdr, uint8_t regAdr) {
	uint8_t reply[4];
	uint8_t n_Byte = 2;

	if (TWI_Start() == 0) {
		//Error
		uart_writeString("TWI Error Start");
		uart_writeAbsatz();
	}
	if (TWI_Write_Addr(i2cAdr, TW_WRITE) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Write_Func(regAdr) == 0) {
		//Error
		uart_writeString("TWI Error regAdr Write");
		uart_writeAbsatz();
	}
	TWI_Stop();

	if (TWI_Start() == 0) {
		//Error
		uart_writeString("TWI Error Start");
		uart_writeAbsatz();
	}
	if (TWI_Write_Addr(i2cAdr, TW_READ) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Read(reply, n_Byte) == 0) {
		//Error
		uart_writeString("TWI Error Read Int");
		uart_writeAbsatz();
	}
	TWI_Stop();
	return (short) reply[0] << 8 | reply[1];
}

void TWI_readRegisterN(uint8_t i2cAdr, uint8_t regAdr,uint8_t reply[], uint8_t n_Byte) {
//	uint8_t n_Byte = 6;
//	uint8_t reply[8];
	if (TWI_Start() == 0) {
		//Error
		uart_writeString("TWI Error Start");
		uart_writeAbsatz();
	}
	if (TWI_Write_Addr(i2cAdr, TW_WRITE) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Write_Func(regAdr) == 0) {
		//Error
		uart_writeString("TWI Error regAdr Write");
		uart_writeAbsatz();
	}
	TWI_Stop();

	if (TWI_Start() == 0) {
		//Error
		uart_writeString("TWI Error Start");
		uart_writeAbsatz();
	}
	if (TWI_Write_Addr(i2cAdr, TW_READ) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Read(reply, n_Byte) == 0) {
		//Error
		uart_writeString("TWI Error Read Int");
		uart_writeAbsatz();
	}
	TWI_Stop();
}
