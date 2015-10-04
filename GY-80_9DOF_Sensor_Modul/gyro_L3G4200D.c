/*
 * gyro_L3G4200D.c
 *
 *  Created on: 01.10.2015
 *      Author: matthias
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <util/twi.h>
#include "twi.h"
#include "uart.h"

#define CTRL_REG1 			0x20
#define CTRL_REG2 			0x21
#define CTRL_REG3 			0x22
#define CTRL_REG4 			0x23
#define CTRL_REG5 			0x24
#define i2cAdr_L3G4200D		0x69
#define scale				2000

short x;
short y;
short z;

void L3G4200D_Init() {
	TWI_Init();

	// Configure L3G4200  - 250, 500 or 2000 deg/sec
	//From  Jim Lindblom of Sparkfun's code

	// Enable x, y, z and turn off power down:
	TWI_writeRegister(i2cAdr_L3G4200D, CTRL_REG1, 0b00001111);

	// If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
	TWI_writeRegister(i2cAdr_L3G4200D, CTRL_REG2, 0b00000000);

	// Configure CTRL_REG3 to generate data ready interrupt on INT2
	// No interrupts used on INT1, if you'd like to configure INT1
	// or INT2 otherwise, consult the datasheet:
	TWI_writeRegister(i2cAdr_L3G4200D, CTRL_REG3, 0b00001000);

	// CTRL_REG4 controls the full-scale range, among other things:

	if (scale == 250) {
		TWI_writeRegister(i2cAdr_L3G4200D, CTRL_REG4, 0b00000000);
	} else if (scale == 500) {
		TWI_writeRegister(i2cAdr_L3G4200D, CTRL_REG4, 0b00010000);
	} else {
		TWI_writeRegister(i2cAdr_L3G4200D, CTRL_REG4, 0b00110000);
	}

	// CTRL_REG5 controls high-pass filtering of outputs, use it
	// if you'd like:
	TWI_writeRegister(i2cAdr_L3G4200D, CTRL_REG5, 0b00000000);
	_delay_ms(100);
}

void L3G4200D_GetValues() {
	char xMSB = TWI_readRegister(i2cAdr_L3G4200D, 0x29);
	char xLSB = TWI_readRegister(i2cAdr_L3G4200D, 0x28);
	x = ((xMSB << 8) | xLSB);

	char yMSB = TWI_readRegister(i2cAdr_L3G4200D, 0x2B);
	char yLSB = TWI_readRegister(i2cAdr_L3G4200D, 0x2A);
	y = ((yMSB << 8) | yLSB);

	char zMSB = TWI_readRegister(i2cAdr_L3G4200D, 0x2D);
	char zLSB = TWI_readRegister(i2cAdr_L3G4200D, 0x2C);
	z = ((zMSB << 8) | zLSB);
}

short L3G4200D_GetX() {
	return x;
}

short L3G4200D_GetY() {
	return y;
}

short L3G4200D_GetZ() {
	return z;
}

