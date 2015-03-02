#include <unistd.h>

#include "../include/libsensors.h"

/*
 * Returns sensors' values
 */
void sensors_get_values(int device, Sensors_values *values)
{
	int buf[2];

	buf[0] = 0x3b;
	write(device, buf, 1);
	read(device, buf, 1);
	values->accX = buf[0] * 256;

	buf[0] = 0x3c;
	write(device, buf, 1);
	read(device, buf, 1);
	values->accX += buf[0];

	buf[0] = 0x3d;
	write(device, buf, 1);
	read(device, buf, 1);
	values->accY = buf[0] * 256;

	buf[0] = 0x3e;
	write(device, buf, 1);
	read(device, buf, 1);
	values->accY += buf[0];

	buf[0] = 0x3f;
	write(device, buf, 1);
	read(device, buf, 1);
	values->accZ = buf[0] * 256;

	buf[0] = 0x40;
	write(device, buf, 1);
	read(device, buf, 1);
	values->accZ += buf[0];

	buf[0] = 0x43;
	write(device, buf, 1);
	read(device, buf, 1);
	values->gyroX = buf[0] * 256;

	buf[0] = 0x44;
	write(device, buf, 1);
	read(device, buf, 1);
	values->gyroX += buf[0];

	buf[0] = 0x45;
	write(device, buf, 1);
	read(device, buf, 1);
	values->gyroY = buf[0] * 256;

	buf[0] = 0x46;
	write(device, buf, 1);
	read(device, buf, 1);
	values->gyroY += buf[0];

	buf[0] = 0x47;
	write(device, buf, 1);
	read(device, buf, 1);
	values->gyroZ = buf[0] * 256;

	buf[0] = 0x48;
	write(device, buf, 1);
	read(device, buf, 1);
	values->gyroZ += buf[0];
}
