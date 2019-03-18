#include <avr/io.h>
#include <avr/interrupt.h>

#include "i2c.h"

void InitI2C1600(){
	I2C1 |= (1 << I2C1);
	I2C2 |= (1 << I2C2);
	I2C3 |= (1 << I2C3);
}

void I2CBluetooth(){
	I2CS2Low();
	I2CS3Low();
}

void I2CUV(){
	I2CS2High();
	I2CS3High();
}

void I2CIR(){
	I2CS2Low();
	I2CS3High();
}

void I2CLight(){
	I2CS2High();
	I2CS3Low();
}
/*
uint32_t I2CTest(){
	CNT = 0x0000;
	TMR = (1 << CS10);

	while(!(I2C3 & (1 << I2C3)));
	TCCR1B = 0x00;

	return ((float)8000000UL / CNT);
}
*/
