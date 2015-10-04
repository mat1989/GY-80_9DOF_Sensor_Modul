/*
 * twi.h
 *
 *  Created on: 24.09.2015
 *      Author: matthias
 */

#ifndef TWI_H_
#define TWI_H_

/*
 * Low Level
 */
uint8_t calcAdr(uint8_t adr, uint8_t modus);
void write(uint8_t u8data);
uint8_t readACK(void);
uint8_t readNACK(void);
uint8_t getStatus(void);

/*
 * Mid Level
 */
void TWI_Init(void);
char TWI_Start(void);
void TWI_Stop(void);
char TWI_Write_Addr(uint8_t i2cAdr, uint8_t i2cModus);
char TWI_Write_Func(uint8_t u8data);
char TWI_Read(uint8_t reply[], uint8_t n_Byte);

/*
 * High Level
 */
void TWI_writeRegister(uint8_t i2cAdr, uint8_t regAdr, uint8_t val);
char TWI_readRegister(uint8_t i2cAdr, uint8_t regAdr);
short TWI_readRegister2(uint8_t i2cAdr, uint8_t regAdr);
void TWI_readRegisterN(uint8_t i2cAdr, uint8_t regAdr,uint8_t reply[], uint8_t n_Byte);


#endif /* TWI_H_ */
