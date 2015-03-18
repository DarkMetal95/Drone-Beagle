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
	int sample, device;
	int buf[2];

	if ((device = open(I2C_DEVICE, O_RDWR)) < 0)
	{
		perror("Failed to open the i2c bus.\n");
		return -1;
	}

	if (ioctl(device, I2C_SLAVE, 0x68) < 0)
	{
		perror("Failed to acquire bus access and/or talk to slave.\n");
		return -1;
	}

	sample = (1000 / I2C_FREQ) - 1;

	buf[0] = 0x6B;
	buf[1] = 0x00;
	write(device, buf, 2);

	buf[0] = 0x1A;
	buf[1] = 0x01;
	write(device, buf, 2);

	buf[0] = 0x1B;
	buf[1] = 0x08;
	write(device, buf, 2);

	buf[0] = 0x19;
	buf[1] = sample;
	write(device, buf, 2);

	return device;
}
