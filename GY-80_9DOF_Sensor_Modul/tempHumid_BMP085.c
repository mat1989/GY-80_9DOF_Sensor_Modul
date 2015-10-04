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

//	uart_writeString("AC1: ");
//	uart_writeInt16(ac1);
//	uart_writeAbsatz();
//	uart_writeString("AC2: ");
//	uart_writeInt16(ac2);
//	uart_writeAbsatz();
//	uart_writeString("AC3: ");
//	uart_writeInt16(ac3);
//	uart_writeAbsatz();
//	uart_writeString("AC4: ");
//	uart_writeInt16(ac4);
//	uart_writeAbsatz();
//	uart_writeString("AC5: ");
//	uart_writeInt16(ac5);
//	uart_writeAbsatz();
//	uart_writeString("AC6: ");
//	uart_writeInt16(ac6);
//	uart_writeAbsatz();
//	uart_writeString("B1: ");
//	uart_writeInt16(b1);
//	uart_writeAbsatz();
//	uart_writeString("B2: ");
//	uart_writeInt16(b2);
//	uart_writeAbsatz();
//	uart_writeString("MB: ");
//	uart_writeInt16(mb);
//	uart_writeAbsatz();
//	uart_writeString("MC: ");
//	uart_writeInt16(mc);
//	uart_writeAbsatz();
//	uart_writeString("MD: ");
//	uart_writeInt16(md);
//	uart_writeAbsatz();
}

// Read the uncompensated temperature value
unsigned int bmp085_ReadUT() {
	unsigned int ut;

	// Write 0x2E into Register 0xF4
	// This requests a temperature reading
//  Wire.beginTransmission(BMP085_ADDRESS);
//  Wire.write(0xF4);
//  Wire.write(0x2E);
//  Wire.endTransmission();
	TWI_writeRegister(i2cAdr_BMP085, 0xF4, 0x2E);
	_delay_ms(50);

	// Read two bytes from registers 0xF6 and 0xF7
	ut = TWI_readRegister2(i2cAdr_BMP085, 0xF6);
//  ut = bmp085ReadInt(0xF6);
	return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085_ReadUP() {
	unsigned char msb, lsb, xlsb;
	unsigned long up = 0;

// 	Write 0x34+(OSS<<6) into register 0xF4
// 	Request a pressure reading w/ oversampling setting
//  Wire.beginTransmission(BMP085_ADDRESS);
//  Wire.write(0xF4);
//  Wire.write(0x34 + (OSS<<6));
//  Wire.endTransmission();
	TWI_writeRegister(i2cAdr_BMP085, 0xF4, (0x34 + (OSS << 6)));
	_delay_ms(50);

	// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
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
void bmp085_GetPressure(unsigned long up, long pressure) {
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
		p = (b7 << 1) / b4;
	} else {
		p = (b7 / b4) << 1;
	}

	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p += (x1 + x2 + 3791) >> 4;

	pressure = p;
//	return p;
//	return 1;
}

float bmp085_calcAltitude(float pressure) {
//	float A = pressure / 101325;
//	float B = 1 / 5.25588;
//	float C = pow(A, B);
//	C = 1 - C;
//	C = C / 0.0000225577;

//	return C;
	return 1;
}

float bmp085_calcAtmosphere(float pressure) {
	return pressure / 101325;
//	return 1;
}
