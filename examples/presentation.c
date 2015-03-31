#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "../include/libsensors.h"
#include "../include/libi2c.h"
#include "../include/libkalman.h"
#include "../include/libbluetooth.h"

int main()
{
	int i2c_device;

	int s, client;
	sdp_session_t *session = NULL;
	sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	char data[11];

	double roll, pitch;
	double dt_time;
	struct timeval t_start, t_end, dt;
	Sensors_values sv;
	Kalman_instance kalman_x, kalman_y;

	int loop;

	/*
	 * Init i2c
	 */

	i2c_device = i2c_setup();

	if (i2c_device < 0)
	{
		perror("An error occurred during i2c setup\n");
		exit(1);
	}

	if (mpu6050_init(i2c_device) != 0)
	{
		perror("An error occurred during MPU6050 setup\n");
		exit(1);
	}

	/*
	 * Init Bluetooth
	 */

	session = bt_register_service();
	s = bt_server_register(&loc_addr);

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

	gettimeofday(&t_start, NULL);

	/*
	 * Init screen
	 */

	initscr();

	/*
	 * Main loop
	 */

	while (1)
	{
		client = bt_server_initiate(s, &rem_addr);
		loop = 1;

		while(loop)
		{
			sensors_get_values(i2c_device, &sv);

			gettimeofday(&t_end, NULL);
			timersub(&t_end, &t_start, &dt);
			dt_time = dt.tv_sec + ((double) dt.tv_usec / 1000000);
			gettimeofday(&t_start, NULL);

			roll = atan(sv.accY / sqrt(sv.accX * sv.accX + sv.accZ * sv.accZ)) * RAD_TO_DEG;
			pitch = atan2(-sv.accX, sv.accZ) * RAD_TO_DEG;

			kalman_x.rate = sv.gyroX / 131.;
			kalman_y.rate = sv.gyroY / 131.;

			if ((pitch < -90 && kalman_y.angle > 90) || (pitch > 90 && kalman_y.angle < -90))
				kalman_y.angle = pitch;
			else
				kalman_compute_new_angle(&kalman_y, pitch, dt_time);

			if (abs(kalman_y.angle) > 90)
				kalman_x.rate = -kalman_x.rate;

			kalman_compute_new_angle(&kalman_x, roll, dt_time);

			mvprintw(0, 0, "pitch : %f", kalman_y.angle);
			mvprintw(1, 0, "roll : %f", kalman_x.angle);
			refresh();

			sprintf(data, "%f",kalman_x.angle);
			write(client, data, strlen(data));
			sprintf(data, "%f",kalman_y.angle);
			write(client, data, strlen(data));

			usleep(2000);
		}	

		close(client);
	}

	i2c_close(i2c_device);

	bt_end_session(s, session);

	return 0;
}
