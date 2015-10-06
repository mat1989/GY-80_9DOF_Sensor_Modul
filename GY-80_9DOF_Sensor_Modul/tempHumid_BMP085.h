/*
 * tempHumid_BMP085.h
 *
 *  Created on: 28.09.2015
 *      Author: matthias
 */

#ifndef TEMPHUMID_BMP085_H_
#define TEMPHUMID_BMP085_H_

//float temperature = bmp085GetTemperature(bmp085ReadUT()); //MUST be called first
//float pressure = bmp085GetPressure(bmp085ReadUP());
//float atm = pressure / 101325; // "standard atmosphere"
//float altitude = calcAltitude(pressure); //Uncompensated caculation - in Meters

void bmp085_Calibration();
unsigned int bmp085_ReadUT();
unsigned long bmp085_ReadUP();
float bmp085_GetTemperature(unsigned int ut);
float bmp085_GetPressure(unsigned long up);
float bmp085_calcAltitude(float pressure);
float bmp085_calcAtmosphere(float pressure);

#endif /* TEMPHUMID_BMP085_H_ */
