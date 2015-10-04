/*
 * main.c
 *
 *  Created on: 28.09.2015
 *      Author: matthias
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "def.h"
#include "twi.h"
#include "uart.h"

#include "tempHumid_BMP085.h"
#include "gyro_L3G4200D.h"
#include "accel_ADXL345.h"
#include "magn_HMC5883L.h"

char status = 0;
char inBuffer[80];

ISR(USART_RXC_vect) {
	uart_readLine(inBuffer, 80);
	status = uart_stringToInt8(inBuffer);
	uart_writeInt8(status);
}


void init(){
	L3G4200D_Init();
	ADXL345_init();
	HMC5883L_init();
	bmp085_Calibration();
}

void bmp085() {
	uart_writeString("TempHumid Sensor BMP085");
	uart_writeAbsatz();

	float temperature = bmp085_GetTemperature(bmp085_ReadUT());
	uart_writeString("Temperatur: ");
	uart_writeFloat(temperature);
	uart_writeString(" °C");
	uart_writeAbsatz();

	float pressure =0;
	bmp085_GetPressure(bmp085_ReadUP(),pressure);
	uart_writeString("Pressure: ");
	uart_writeFloat(pressure);
	uart_writeString(" Pa / ");
//	float pressure2 = pressure / 10.0;
//	uart_writeFloat(pressure2);
//	uart_writeString(" hPa");
//	uart_writeAbsatz();
//
//	float atm = bmp085_calcAtmosphere(pressure);
//	uart_writeString("Atmosphere: ");
//	uart_writeFloat(atm);
//	uart_writeAbsatz();

	//	float altitude = bmp085_calcAltitude(pressure);
	//	uart_writeString("Altitude: ");
	//	uart_writeFloat(altitude);
	//	uart_writeAbsatz();
}

void L3G4200D() {
//	uart_writeString("Gyro L3G4200D");
//	uart_writeAbsatz();

	L3G4200D_GetValues();
	short x = L3G4200D_GetX();
	short y = L3G4200D_GetY();
	short z = L3G4200D_GetZ();

	uart_writeString("X: ");
	uart_writeInt16(x);
	uart_writeString(" Y: ");
	uart_writeInt16(y);
	uart_writeString(" Z: ");
	uart_writeInt16(z);
	uart_writeAbsatz();
}

void ADXL345() {
//	uart_writeString("Accelerometer ADXL345");
//	uart_writeAbsatz();

	ADXL345_read_accel();
	int16_t accelX = ADXL345_getX();
	int16_t accelY = ADXL345_getY();
	int16_t accelZ = ADXL345_getZ();

	uart_writeString("X: ");
	uart_writeInt16(accelX);
	uart_writeString(" /Y: ");
	uart_writeInt16(accelY);
	uart_writeString(" /Z: ");
	uart_writeInt16(accelZ);
	uart_writeAbsatz();
}

void HMC5883L() {
//	uart_writeString("Magnetometer HMC5883L");
//	uart_writeAbsatz();

	HMC5883L_ReadScaledAxis();
	int16_t magnX = HMC5883L_GetX();
	int16_t magnY = HMC5883L_GetY();
	int16_t magnZ = HMC5883L_GetZ();

	uart_writeString("X: ");
	uart_writeInt16(magnX);
	uart_writeString(" /Y: ");
	uart_writeInt16(magnY);
	uart_writeString(" /Z: ");
	uart_writeInt16(magnZ);
	uart_writeAbsatz();
}

int main() {
	uart_init();
	sei();
	uart_writeAbsatz();
	uart_writeAbsatz();
	uart_writeString("GY-80_9DOF Sensor Modul");
	uart_writeAbsatz();
	init();

	while (1) {
		switch(status){
		case 0:
			break;
		case 1:
			L3G4200D();
			break;
		case 2:
			ADXL345();
			break;
		case 3:
			HMC5883L();
			break;
		case 4:
			break;
		}
//		//bmp085();
//		L3G4200D();
//		ADXL345();
//		HMC5883L();
//		uart_writeAbsatz();
//		uart_writeAbsatz();
		_delay_ms(300);
	}
	return 0;
}
