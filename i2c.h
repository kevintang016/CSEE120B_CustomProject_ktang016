#ifndef I2C1600_H_
#define I2C1600_H_

//SCL Connection
#define I2C2_DDR DDRC
#define I2C2_PORT PORTC
#define I2C2_POS PC0

//SDA Connection
#define I2C3_DDR DDRC
#define I2C3_PORT PORTC
#define I2C3_POS PC1

#define	I2CS2High()	I2C2_PORT |= (1 << I2C2_POS)
#define	I2CS2Low()	I2C2_PORT &= ~(1 << I2C2_POS)
#define	I2CS3High()	I2C3_PORT |= (1 << I2C3_POS)
#define	I2CS3Low()	I2C3_PORT &= ~(1 << I2C3_POS)

void InitI2C1600();
void I2CBluetooth();
void I2CUV();
void I2CIR();
void I2CLight();

#endif 
