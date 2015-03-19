#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../include/libsensors.h"
#include "../include/libi2c.h"
#include "../include/libkalman.h"

int main()
{
	int i2c_device;

	double roll, pitch;
	double dt;
	Sensors_values sv;
	Kalman_instance kalman_x, kalman_y;

	/*
	 * Init i2c
	 */

	i2c_device = i2c_setup();

	if (i2c_device == -1)
	{
		perror("An error occurred when opening the I2C bus\n");
		exit(1);
	}

	/*
	 * Init Kalman filter
	 */

	kalman_init(&kalman_x);
	kalman_init(&kalman_y);

	// Wait for sensor to stabilize
	sleep(1000);
	
	sensors_get_values(i2c_device, &sv);

	roll = atan(sv.accY / sqrt(sv.accX * sv.accX + sv.accZ * sv.accZ)) * RAD_TO_DEG;
	pitch = atan2(-sv.accX, sv.accZ) * RAD_TO_DEG;

	kalman_x.angle = roll;
	kalman_y.angle = pitch;

	// timer = ;
	
	while (1)
	{
		sensors_get_values(i2c_device, &sv);

		//dt = ;
		//timer = ;

		roll = atan(sv.accY / sqrt(sv.accX * sv.accX + sv.accZ * sv.accZ)) * RAD_TO_DEG;
		pitch = atan2(-sv.accX, sv.accZ) * RAD_TO_DEG;

		kalman_x.rate = sv.gyroX / 131.;
		kalman_y.rate = sv.gyroY / 131.;

		if ((pitch < -90 && kalman_y.angle > 90) || (pitch > 90 && kalman_y.angle < -90))
			kalman_y.angle = pitch;
		else
			kalman_compute_new_angle(&kalman_y, pitch, kalman_y.rate, dt);

		if (abs(kalman_y.angle) > 90)
			kalman_x.rate = -kalman_x.rate;

		kalman_compute_new_angle(&kalman_x, roll, kalman_x.rate, dt);

		if (sv.gyroX < -180 || sv.gyroX > 180)
			sv.gyroX = kalman_x.angle;

		if (sv.gyroY < -180 || sv.gyroY > 180)
			sv.gyroY = kalman_y.angle;
	}
}
