/*
 * magn_HMC5883L.h
 *
 *  Created on: 01.10.2015
 *      Author: matthias
 */

#ifndef MAGN_HMC5883L_H_
#define MAGN_HMC5883L_H_

#define i2cAdr_HMC5883L 0x1E
#define ConfigurationRegisterA 0x00
#define ConfigurationRegisterB 0x01
#define ModeRegister 0x02
#define DataRegisterBegin 0x03

#define Measurement_Continuous 0x00
#define Measurement_SingleShot 0x01
#define Measurement_Idle 0x03

#define PI 3.14159265359
#define RAD_TO_DEG PI/180

/*
 * Serial.println("Setting scale to +/- 1.3 Ga");
 * error = compass.SetScale(1.3); // Set the scale of the compass.
 *
 * Serial.println("Setting measurement mode to continous.");
 * error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
 */

void HMC5883L_init();
void HMC5883L_ReadScaledAxis();

float HMC5883L_GetX();
float HMC5883L_GetY();
float HMC5883L_GetZ();
//float HMC5883L_GetHeading();

void HMC5883L_SetScale(float gauss);
void HMC5883L_SetMeasurementMode(uint8_t mode);

#endif /* MAGN_HMC5883L_H_ */
