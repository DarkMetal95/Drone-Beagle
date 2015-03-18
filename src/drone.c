#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include "../include/libbluetooth.h"
#include "../include/libi2c.h"
#include "../include/libsensors.h"
#include "../include/libio.h"
#include "../include/libpwm.h"
#include "../include/libkalman.h"

#define INCREMENT			1000

int main()
{

	/*
	 * Variables
	 */
	
	int i = 0, flag = 0, i2c_device;
	int s, client;
	int buf[2];
	char data_char[6];
	float dt_time, roll, pitch;
	clock_t dt;
	Sensors_values sensors_values;
	Kalman_instance kalmanx, kalmany;
	sdp_session_t *session = NULL;
	sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

	/*
	 * Setup
	 */

	i2c_device = i2c_setup();
	session = bt_register_service();
	s = bt_server_register(&loc_addr);

	kalman_init(&kalmanx);
	kalman_init(&kalmany);
	sensors_get_values(i2c_device, &sensors_values);

	roll = atan2(sensors_values.accY, sensors_values.accZ) * RAD_TO_DEG;
	pitch = atan(-sensors_values.accX / sqrt(sensors_values.accY * sensors_values.accY + sensors_values.accZ * sensors_values.accZ)) * RAD_TO_DEG;

	kalmanx.angle = roll;
	kalmany.angle = pitch;

	dt = clock();

	/*
	 * Main loop
	 */

	while (1)
	{
		client = bt_server_initiate(s, &rem_addr);

		while (flag >= 0)
		{
			sensors_get_values(i2c_device, &sensors_values);
			
			dt_time = (float) dt / (CLOCKS_PER_SEC);
			dt = clock();

			//computeNewAngle(Kalman_instance *instance, float newAngle, float newRate, float dt);

			roll = atan2(sensors_values.accY, sensors_values.accZ) * RAD_TO_DEG;
			pitch = atan(-sensors_values.accX / sqrt(sensors_values.accY * sensors_values.accY + sensors_values.accZ * sensors_values.accZ)) * RAD_TO_DEG;

			sensors_values.gyroX /= 131;
			sensors_values.gyroY /= 131;
			sensors_values.gyroZ /= 131;
			
			// printf("AccX = %f\t AccY = %f\t AccZ = %f\nGyroX = %f\t GyroY = 
			// 
			// 
			// 
			// %f\t GyroZ = %f\n\n",accx, accy, accz, gyrox, gyroy, gyroz);

			snprintf(data_char, sizeof(data_char), "%d", sensors_values.accX);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", sensors_values.accY);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", sensors_values.accZ);
			write(client, data_char, 6);

			snprintf(data_char, sizeof(data_char), "%d", sensors_values.gyroX);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", sensors_values.gyroY);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", sensors_values.gyroZ);
			flag = write(client, data_char, 6);

			// prev = stop;
			stop = clock();

			stop_time = (double)stop / (CLOCKS_PER_SEC / 1000000);
			// prev_time = (double) prev / (CLOCKS_PER_SEC / 1000000);

			usleep(((1 / I2C_FREQ) * 1000000) - (stop_time - start_time));
			i++;
			printf("Paquets: %d\r", i);
			// sample_frequency = 1/((stop_time/1000000)-(prev_time/1000000));

			// printf("Start: %f\tStop: %f\t Prev: %f\t Sampling frequency:
			// %f\n", start_time, stop_time, prev_time, sample_frequency);
		}

		i = 0;
		flag = 0;
		close(client);
		printf("\nDisconnected\n");
	}

	bt_end_session(s, session);

	return 0;
}