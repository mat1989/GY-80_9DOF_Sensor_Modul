/*
 * magn_HMC5883L.c
 *
 *  Created on: 01.10.2015
 *      Author: matthias
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/twi.h>
#include "magn_HMC5883L.h"
#include "twi.h"
#include "uart.h"

char m_Scale = 1;
int16_t raw[3];
int16_t scaled[3];

void HMC5883L_init() {
	TWI_Init();
}

void HMC5883L_ReadScaledAxis() {
	uint8_t buffer[8]; // = Read(DataRegisterBegin, 6);
	TWI_readRegisterN(i2cAdr_HMC5883L, DataRegisterBegin, buffer, 6);
	raw[0] = (buffer[0] << 8) | buffer[1]; //X Axis
	raw[1] = (buffer[2] << 8) | buffer[3]; //Z Axis
	raw[2] = (buffer[4] << 8) | buffer[5]; //Y Axis

	scaled[0] = raw[0] * m_Scale; //X Axis
	scaled[1] = raw[1] * m_Scale; //Z Axis
	scaled[2] = raw[2] * m_Scale; //Y Axis
}

int16_t HMC5883L_GetX() {
	return scaled[0];
}

int16_t HMC5883L_GetY() {
	return scaled[2];
}

int16_t HMC5883L_GetZ() {
	return scaled[1];
}

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
