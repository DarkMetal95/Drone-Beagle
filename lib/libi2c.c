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
	{
		perror("Failed to open the i2c bus.\n");
		return -1;
	}

	if (ioctl(device, I2C_SLAVE, 0x68) < 0)
	{
		perror("Failed to acquire bus access and/or talk to slave.\n");
		return -2;
	}

	// Freq = 8000 / (1 + MPU6050_FREQ_DIV)
	if (i2c_write_byte(device, 0x19, MPU6050_FREQ_DIV) != 0)
		return -3;
	// Disable Fsync, set 260 Hz Acc filtering
	if (i2c_write_byte(device, 0x1A, 0x00) != 0)
		return -3;
	// Set gyro full scale range to +-250deg/s
	if (i2c_write_byte(device, 0x1B, 0x00) != 0)
		return -3;
	// Set acc full scale range to +-2g
	if (i2c_write_byte(device, 0x1C, 0x00) != 0)
		return -3;
	// Wake up device
	if (i2c_write_byte(device, 0x6B, 0x01) != 0)
		return -3;

	return device;
}

int i2c_write_byte(int device, int address, int byte)
{
	int buf[2];

	buf[0] = address;
	buf[1] = byte;

	if (write(device, buf, 2) != 2)
		return -1;

	return 0;
}
