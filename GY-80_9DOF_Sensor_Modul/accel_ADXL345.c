/*
 * accel_ADXL345.c
 *
 *  Created on: 28.09.2015
 *      Author: matthias
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/twi.h>
#include <math.h>
#include "accel_ADXL345.h"
#include "twi.h"
#include "uart.h"

#define i2cAdr_ADXL345 	0x53	// ADXL345 device address
#define ADXL345_TO_READ	6		// num of bytes we are going to read each time (two bytes for each axis)

// Gain for gyroscope
#define GYRO_GAIN_X (0.06957f)
#define GYRO_GAIN_Y (0.06957f)
#define GYRO_GAIN_Z (0.06957f)

#define GYRO_X_SCALE (TO_RAD(GYRO_GAIN_X))
#define GYRO_Y_SCALE (TO_RAD(GYRO_GAIN_Y))
#define GYRO_Z_SCALE (TO_RAD(GYRO_GAIN_Z))


void ADXL345_init() {
	TWI_Init();
	//Turning on the ADXL345
	TWI_writeRegister(i2cAdr_ADXL345, 0x2D, 0x08);
	TWI_writeRegister(i2cAdr_ADXL345, 0x31, 0x08);
	TWI_writeRegister(i2cAdr_ADXL345, 0x2C, 0x09);
}

int16_t accel[3];
void ADXL345_read_accel(){
	uint8_t reply[8];
	TWI_readRegisterN(i2cAdr_ADXL345, 0x32, reply, 6);
	accel[0] = (((int16_t) reply[1]) << 8) | reply[0]; // Y axis (internal sensor x axis)
	accel[1] = (((int16_t) reply[3]) << 8) | reply[2]; // X axis (internal sensor y axis)
	accel[2] = (((int16_t) reply[5]) << 8) | reply[4]; // Z axis (internal sensor z axis)
}

int16_t ADXL345_getX(){
	return accel[1];
}

int16_t ADXL345_getY(){
	return accel[0];
}

int16_t ADXL345_getZ(){
	return accel[2];
}

// Sets the OFSX, OFSY and OFSZ bytes
// OFSX, OFSY and OFSZ are user offset adjustments in twos complement format with
// a scale factor of 15,6mg/LSB
// OFSX, OFSY and OFSZ should be comprised between
void ADXL345_setAxisOffset(char x, char y, char z) {
	TWI_writeRegister(i2cAdr_ADXL345,0x1E,x);
	TWI_writeRegister(i2cAdr_ADXL345,0x1F,y);
	TWI_writeRegister(i2cAdr_ADXL345,0x20,z);
}

// Gets the OFSX, OFSY and OFSZ bytes
void ADXL345_getAxisOffset(char x, char y, char z) {
	x = TWI_readRegister(i2cAdr_ADXL345,0x1E);
	y = TWI_readRegister(i2cAdr_ADXL345,0x1F);
	z = TWI_readRegister(i2cAdr_ADXL345,0x20);
}

