#include <avr/io.h>
#include <avr/interrupt.h>

#include "si1145test.h"

void InitI2C3200(){
	I2C_S2_DDR|=(1<<I2C_S2_POS);
	I2C_S3_DDR|=(1<<I2C_S3_POS);
	I2C_LED_DDR|=(1<<I2C_LED_POS);
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

uint32_t I2CMeasure(){
	if(!(I2C_OUT_PORT & (1<<I2C_OUT_POS))) {
		while(!(I2C_OUT_PORT & (1<<I2C_OUT_POS)));
	}

	while(I2C_OUT_PORT & (1<<I2C_OUT_POS));

	CNT=0x0000;
	TMR=(1<<CS10);

	while(!(I2C_OUT_PORT & (1<<I2C_OUT_POS)));
	TCCR1B=0x00; 
	
	return ((float)8000000UL/CNT);
}
