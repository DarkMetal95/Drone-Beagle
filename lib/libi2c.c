#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "../include/libi2c.h"

/*
 * Returns I2C descriptor of I2C_DEVICE
 */
int i2c_setup()
{
	int device;

	if ((device = open(I2C_DEVICE, O_RDWR)) < 0)
		return -1;

	return device;
}

void i2c_close(int device)
{
	close(device);
}

int i2c_set_address(int device, unsigned char address)
{
	if (ioctl(device, I2C_SLAVE, address) < 0)
		return -1;

	return 0;
}

int i2c_write_byte(int device, unsigned char device_addr, unsigned char reg_addr, unsigned char byte)
{
	unsigned char buf[2];

	i2c_set_address(device, device_addr);

	buf[0] = reg_addr;
	buf[1] = byte;

	if (write(device, buf, 2) != 2)
		return -1;

	return 0;
}

int i2c_read_byte(int device, unsigned char device_addr, unsigned char reg_addr)
{
	unsigned char buf[1];

	i2c_set_address(device, device_addr);

	if (write(device, &reg_addr, 1) != 1)
		return -1;

	i2c_set_address(device, device_addr);

	if (read(device, buf, 1) != 1)
		return -1;

	return buf[0];
}

int i2c_read_bytes(int device, unsigned char device_addr, unsigned char reg_addr, unsigned char *read_buffer, size_t n)
{
	i2c_set_address(device, device_addr);

	if (write(device, &reg_addr, 1) != 1)
		return -1;

	i2c_set_address(device, device_addr);

	if (read(device, read_buffer, n) != n)
		return -1;

	return 0;
}

int mpu6050_init(int device)
{
	// Freq = 8000 / (1 + MPU6050_FREQ_DIV)
	if (i2c_write_byte(device, MPU6050_ADDR, 0x19, MPU6050_FREQ_DIV) != 0)
		return -3;
	// Disable Fsync, set 260 Hz Acc filtering
	if (i2c_write_byte(device, MPU6050_ADDR, 0x1A, 0x00) != 0)
		return -3;
	// Set gyro full scale range to +-250deg/s
	if (i2c_write_byte(device, MPU6050_ADDR, 0x1B, 0x00) != 0)
		return -3;
	// Set acc full scale range to +-2g
	if (i2c_write_byte(device, MPU6050_ADDR, 0x1C, 0x00) != 0)
		return -3;
	// Wake up device
	if (i2c_write_byte(device, MPU6050_ADDR, 0x6B, 0x01) != 0)
		return -3;

	return 0;
}
