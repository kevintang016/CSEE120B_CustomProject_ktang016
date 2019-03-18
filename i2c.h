#ifndef I2C3200_H_
#define I2C3200_H_

//SCL Connection
#define I2C_S2_DDR DDRC
#define I2C_S2_PORT PORTC
#define I2C_S2_POS PC1

//SDA Connection
#define I2C_S3_DDR DDRC
#define I2C_S3_PORT PORTC
#define I2C_S3_POS PC2

#define	I2CS2High()	I2C_S2_PORT|=(1<<I2C_S2_POS)
#define	I2CS2Low()	I2C_S2_PORT&=~(1<<I2C_S2_POS)
#define	I2CS3High()	I2C_S3_PORT|=(1<<I2C_S3_POS)
#define	I2CS3Low()	I2C_S3_PORT&=~(1<<I2C_S3_POS)

void InitI2C3200();

void I2CBluetooth();
void I2CUV();
void I2CIR();
void I2CLight();

uint32_t I2CMeasure();

#endif /* I2C3200_H_ */