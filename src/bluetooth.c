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

#include "../include/libbluetooth.h"
#include "../include/libi2c.h"
#include "../include/libsensors.h"

int main()
{
	int i = 0, flag = 0, i2c;
	int s, client;
	int buf[2];
	char data_char[6];
	double start_time, stop_time;
	clock_t start, stop;
	Sensors_values sensors_values;
	sdp_session_t *session = NULL;
	sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

	i2c = i2c_setup();
	session = bt_register_service();
	s = bt_server_register(&loc_addr);

	while (1)
	{
		client = bt_server_initiate(s, &rem_addr);

		while (flag >= 0)
		{
			start = clock();

			sensors_get_values(&sensors_values);

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

			start_time = (double)start / (CLOCKS_PER_SEC / 1000000);
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
