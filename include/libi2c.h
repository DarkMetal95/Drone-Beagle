#ifndef I2C_H
#define i2C_H

#include <stdlib.h>

/*
 * Defines
 */

#define I2C_DEVICE					"/dev/i2c-1"

#define MPU6050_ADDR					0x68
#define MPU6050_FREQ_DIV				7
#define MPU6050_START_ADDR_DATA			0x3B

/*
 * Prototypes
 */

int i2c_setup();
void i2c_close(int device);
int i2c_set_address(int device, unsigned char address);
int i2c_read_byte(int device, unsigned char device_addr, unsigned char reg_addr);
int i2c_write_byte(int device, unsigned char device_addr, unsigned char reg_addr, unsigned char byte);
int i2c_read_bytes(int device, unsigned char device_addr, unsigned char reg_addr, unsigned char *read_buffer, size_t n);

int mpu6050_init(int device);

#endif
