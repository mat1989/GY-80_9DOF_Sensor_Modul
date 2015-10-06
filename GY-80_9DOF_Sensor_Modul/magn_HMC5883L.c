/*
 * magn_HMC5883L.c
 *
 *  Created on: 01.10.2015
 *      Author: matthias
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/twi.h>
#include <math.h>
#include "magn_HMC5883L.h"
#include "twi.h"
#include "uart.h"

float m_Scale = 1;

float magnX;
float magnY;
float magnZ;
float heading;

void HMC5883L_init() {
	TWI_Init();
}

void HMC5883L_ReadScaledAxis() {
	int8_t buffer[8];

	if (TWI_Start() == 0) {
		//Error
		uart_writeString("TWI Error Start");
		uart_writeAbsatz();
	}
	if (TWI_Write_Addr(i2cAdr_HMC5883L, TW_WRITE) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Write_Func(DataRegisterBegin) == 0) {
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
	if (TWI_Write_Addr(i2cAdr_HMC5883L, TW_READ) == 0) {
		//Error
		uart_writeString("TWI Error i2cAdr Write");
		uart_writeAbsatz();
	}
	if (TWI_Read_sign(buffer, 6) == 0) {
		//Error
		uart_writeString("TWI Error Read Int");
		uart_writeAbsatz();
	}
	TWI_Stop();

	magnX = ((buffer[0] << 8) | buffer[1]) * m_Scale; //X Axis
	magnZ = ((buffer[2] << 8) | buffer[3]) * m_Scale; //Z Axis
	magnY = ((buffer[4] << 8) | buffer[5]) * m_Scale; //Y Axis
}

float HMC5883L_GetX() {
	return magnX;
}

float HMC5883L_GetY() {
	return magnY;
}

float HMC5883L_GetZ() {
	return magnZ;
}

//float HMC5883L_GetHeading(){
//  heading = atan2(magnY, magnX);
//
//  // Correct for when signs are reversed.
//  if(heading < 0) heading += 2*PI;
//  if(heading > 2*PI) heading -= 2*PI;
//
//  return heading * RAD_TO_DEG; //radians to degrees
//}

void HMC5883L_SetScale(float gauss) {
	uint8_t regValue = 0x00;
	if (gauss == 0.88) {
		regValue = 0x00;
		m_Scale = 0.73;
	} else if (gauss == 1.3) {
		regValue = 0x01;
		m_Scale = 0.92;
	} else if (gauss == 1.9) {
		regValue = 0x02;
		m_Scale = 1.22;
	} else if (gauss == 2.5) {
		regValue = 0x03;
		m_Scale = 1.52;
	} else if (gauss == 4.0) {
		regValue = 0x04;
		m_Scale = 2.27;
	} else if (gauss == 4.7) {
		regValue = 0x05;
		m_Scale = 2.56;
	} else if (gauss == 5.6) {
		regValue = 0x06;
		m_Scale = 3.03;
	} else if (gauss == 8.1) {
		regValue = 0x07;
		m_Scale = 4.35;
	} else {

	}
	// Setting is in the top 3 bits of the register.
	regValue = regValue << 5;
	TWI_writeRegister(i2cAdr_HMC5883L, ConfigurationRegisterB, regValue);
}

void HMC5883L_SetMeasurementMode(uint8_t mode) {
	TWI_writeRegister(i2cAdr_HMC5883L, ModeRegister, mode);
}
