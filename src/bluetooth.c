#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <../include/libbluetooth.h>

#define FREQ 30

int i2c;
int buf[2];

void setup_i2c()
{
	int sample;

	char *filename = "/dev/i2c-1";
	if ((i2c = open(filename, O_RDWR)) < 0)
	{
		perror("Failed to open the i2c bus");
		exit(1);
	}
	if (ioctl(i2c, I2C_SLAVE, 0x68) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		exit(1);
	}

	sample = (1000 / FREQ) - 1;

	buf[0] = 0x6B;
	buf[1] = 0x00;
	write(i2c, buf, 2);

	buf[0] = 0x1A;
	buf[1] = 0x01;
	write(i2c, buf, 2);

	buf[0] = 0x1B;
	buf[1] = 0x08;
	write(i2c, buf, 2);

	buf[0] = 0x19;
	buf[1] = sample;
	write(i2c, buf, 2);
}

int main()
{
	clock_t start, stop;
	double start_time, stop_time;
	char data_char[6];
	int accx, accy, accz;
	int gyrox, gyroy, gyroz;
	int flag = 0;

	int i = 0;

	setup_i2c();
	bt_server_register();

	while (1)
	{
		bt_server_initiate();

		while (flag >= 0)
		{
			start = clock();

			buf[0] = 0x3b;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accx = buf[0] * 256;

			buf[0] = 0x3c;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accx += buf[0];

			buf[0] = 0x3d;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accy = buf[0] * 256;

			buf[0] = 0x3e;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accy += buf[0];

			buf[0] = 0x3f;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accz = buf[0] * 256;

			buf[0] = 0x40;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accz += buf[0];

			buf[0] = 0x43;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyrox = buf[0] * 256;

			buf[0] = 0x44;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyrox += buf[0];

			buf[0] = 0x45;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyroy = buf[0] * 256;

			buf[0] = 0x46;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyroy += buf[0];

			buf[0] = 0x47;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyroz = buf[0] * 256;

			buf[0] = 0x48;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyroz += buf[0];

			// printf("AccX = %f\t AccY = %f\t AccZ = %f\nGyroX = %f\t GyroY = 
			// 
			// 
			// 
			// %f\t GyroZ = %f\n\n",accx, accy, accz, gyrox, gyroy, gyroz);

			snprintf(data_char, sizeof(data_char), "%d", accx);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", accy);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", accz);
			write(client, data_char, 6);

			snprintf(data_char, sizeof(data_char), "%d", gyrox);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", gyroy);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", gyroz);
			flag = write(client, data_char, 6);

			// prev = stop;
			stop = clock();

			start_time = (double)start / (CLOCKS_PER_SEC / 1000000);
			stop_time = (double)stop / (CLOCKS_PER_SEC / 1000000);
			// prev_time = (double) prev / (CLOCKS_PER_SEC / 1000000);

			usleep(((1 / FREQ) * 1000000) - (stop_time - start_time));
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
	end();
	return 0;
}
