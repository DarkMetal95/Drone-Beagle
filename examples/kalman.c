#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <ncurses.h>

#include "../include/libsensors.h"
#include "../include/libi2c.h"
#include "../include/libkalman.h"

int main()
{
	int i2c_device;

	double roll, pitch;
	double dt;
	clock_t t_start, t_end;
	Sensors_values sv;
	Kalman_instance kalman_x, kalman_y;

	/*
	 * Init i2c
	 */

	i2c_device = i2c_setup();

	if (i2c_device < 0)
	{
		perror("An error occurred during i2c setup\n");
		exit(1);
	}

	/*
	 * Init Kalman filter
	 */

	kalman_init(&kalman_x);
	kalman_init(&kalman_y);

	// Wait 100 ms for sensor to stabilize
	usleep(100000);
	
	sensors_get_values(i2c_device, &sv);

	roll = atan(sv.accY / sqrt(sv.accX * sv.accX + sv.accZ * sv.accZ)) * RAD_TO_DEG;
	pitch = atan2(-sv.accX, sv.accZ) * RAD_TO_DEG;

	kalman_x.angle = roll;
	kalman_y.angle = pitch;

	t_start = clock();

	/*
	 * Init screen
	 */

	initscr();

	/*
	 * Main loop
	 */

	while (1)
	{
		sensors_get_values(i2c_device, &sv);

		t_end = clock();
		dt = 1000000. / CLOCKS_PER_SEC * (t_end - t_start);
		t_start = clock();

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

		mvprintw(0, 0, "pitch : %f", kalman_y.angle);
		mvprintw(1, 0, "roll : %f", kalman_x.angle);
		refresh();

		usleep(2000);
	}
}
