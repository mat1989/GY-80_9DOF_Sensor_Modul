/*
 * tempHumid_BMP085.c
 *
 *  Created on: 28.09.2015
 *      Author: matthias
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <util/twi.h>
#include <math.h>
#include "twi.h"
#include "uart.h"

#define i2cAdr_BMP085 0x77  // I2C address of BMP085
const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
short ac1;
short ac2;
short ac3;
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1;
short b2;
short mb;
short mc;
short md;
// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5;

// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void bmp085_Calibration() {
	ac1 = TWI_readRegister2(i2cAdr_BMP085, 0xAA);
	ac2 = TWI_readRegister2(i2cAdr_BMP085, 0xAC);
	ac3 = TWI_readRegister2(i2cAdr_BMP085, 0xAE);
	ac4 = TWI_readRegister2(i2cAdr_BMP085, 0xB0);
	ac5 = TWI_readRegister2(i2cAdr_BMP085, 0xB2);
	ac6 = TWI_readRegister2(i2cAdr_BMP085, 0xB4);
	b1 = TWI_readRegister2(i2cAdr_BMP085, 0xB6);
	b2 = TWI_readRegister2(i2cAdr_BMP085, 0xB8);
	mb = TWI_readRegister2(i2cAdr_BMP085, 0xBA);
	mc = TWI_readRegister2(i2cAdr_BMP085, 0xBC);
	md = TWI_readRegister2(i2cAdr_BMP085, 0xBE);
}

// Read the uncompensated temperature value
unsigned int bmp085_ReadUT() {
	unsigned int ut;
	TWI_writeRegister(i2cAdr_BMP085, 0xF4, 0x2E);
	_delay_ms(50);
	ut = TWI_readRegister2(i2cAdr_BMP085, 0xF6);
	return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085_ReadUP() {
	unsigned char msb, lsb, xlsb;
	unsigned long up = 0;
	TWI_writeRegister(i2cAdr_BMP085, 0xF4, (0x34 + (OSS << 6)));
	_delay_ms(50);
	msb = TWI_readRegister(i2cAdr_BMP085, 0xF6);
	lsb = TWI_readRegister(i2cAdr_BMP085, 0xF7);
	xlsb = TWI_readRegister(i2cAdr_BMP085, 0xF8);

	up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8)
			| (unsigned long) xlsb) >> (8 - OSS);

	return up;
}

// Calculate temperature in deg C
float bmp085_GetTemperature(unsigned int ut) {
	long x1, x2;

	x1 = (((long) ut - (long) ac6) * (long) ac5) >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;

	float temp = ((b5 + 8) >> 4);
	temp = temp / 10;

	return temp;
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
float bmp085_GetPressure(unsigned long up) {
	long x1, x2, x3, b3, b6, p;
	unsigned long b4, b7;

	b6 = b5 - 4000;
	// Calculate B3
	x1 = (b2 * (b6 * b6) >> 12) >> 11;
	x2 = (ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((((long) ac1) * 4 + x3) << OSS) + 2) >> 2;

	// Calculate B4
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;

	b7 = ((unsigned long) (up - b3) * (50000 >> OSS));
	if (b7 < 0x80000000) {
		p = (b7 *2) / b4;
	} else {
		p = (b7 / b4) *2;
	}

	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p += ((x1 + x2 + 3791) >> 4);

	return (p/10);
}

float bmp085_calcAltitude(float pressure) {
//	float A = pressure / 101325;
//	float B = 0.1902630958; //1 / 5.25588;
//	float C = pow(A, B);
//	C = 1 - C;
//	C = C / 0.0000225577;
//
//	return C;
	return 1;
}

float bmp085_calcAtmosphere(float pressure) {
	return pressure / 101325;
}
