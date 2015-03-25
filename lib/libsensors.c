#include "../include/libi2c.h"
#include "../include/libsensors.h"

/*
 * Returns sensors' values
 */
void sensors_get_values(int device, Sensors_values *values)
{
	unsigned char buf[14];

	i2c_read_bytes(device, MPU6050_ADDR, MPU6050_START_ADDR_DATA, buf, 14);

	values->accX = (buf[0] << 8) | buf[1];
	values->accY = (buf[2] << 8) | buf[3];
	values->accZ = (buf[4] << 8) | buf[5];
	values->gyroX = (buf[8] << 8) | buf[9];
	values->gyroY = (buf[10] << 8) | buf[11];
	values->gyroZ = (buf[12] << 8) | buf[13];
}
