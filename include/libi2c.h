#ifndef I2C_H
#define i2C_H

/*
 * Defines
 */

#define I2C_DEVICE			"/dev/i2c-1"

#define MPU6050_FREQ_DIV		7

/*
 * Prototypes
 */

int i2c_setup();
int i2c_write_byte(int device, int address, int byte);

#endif
