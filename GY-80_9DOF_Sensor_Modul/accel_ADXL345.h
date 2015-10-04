/*
 * accel_ADXL345.h
 *
 *  Created on: 28.09.2015
 *      Author: matthias
 */

#ifndef ACCEL_ADXL345_H_
#define ACCEL_ADXL345_H_

void ADXL345_init();
void ADXL345_read_accel();
int16_t ADXL345_getX();
int16_t ADXL345_getY();
int16_t ADXL345_getZ();

void ADXL345_setAxisOffset(char x, char y, char z);
void ADXL345_getAxisOffset(char x, char y, char z);

#endif /* ACCEL_ADXL345_H_ */
