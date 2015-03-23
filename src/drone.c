#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "../include/libbluetooth.h"
#include "../include/libpwm.h"
#include "../include/libsensors.h"
#include "../include/libi2c.h"
#include "../include/libkalman.h"

/*
 * Global variables
 */

int i2c_device;

double roll, pitch;
double dt;
time_t t_start, t_end;
Sensors_values sv;
Kalman_instance kalman_x, kalman_y;
int kp, ki, kd;

void *compute_kalman_filter()
{
	while (1)
	{
		sensors_get_values(i2c_device, &sv);

		t_end = clock();
		dt = (t_end - t_start) / CLOCKS_PER_SEC;
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

		sleep(2);
	}

	return NULL;
}

int main()
{
	int debug = 0;

	int end_b = 1, end_w = 0, shut = 1;

	int s, client;
	char key[1] = { 0x00 };
	char coef[5] = { 0x00 }; 
	char data[4];
	char debug_inf[50];
	sdp_session_t *session = NULL;
	sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

	long int speed1 = (long int)PWM_SPEED;
	long int speed2 = (long int)PWM_SPEED;
	long int speed3 = (long int)PWM_SPEED;
	long int speed4 = (long int)PWM_SPEED;
	FILE *motor1 = NULL, *motor2 = NULL, *motor3 = NULL, *motor4 = NULL;

	pthread_t tid;

	int x_angle_cons, y_angle_cons;

	/*
	 * Init motors
	 */

	motor1 = fopen(PWM_DUTY_FILE1, "w");
	motor2 = fopen(PWM_DUTY_FILE2, "w");
	motor3 = fopen(PWM_DUTY_FILE3, "w");
	motor4 = fopen(PWM_DUTY_FILE4, "w");

	if (motor1 == NULL)
	{
		perror("An error occurred when opening the motor1\n");
		exit(1);
	}

	if (motor2 == NULL)
	{
		perror("An error occurred when opening the motor2\n");
		exit(1);
	}

	if (motor3 == NULL)
	{
		perror("An error occurred when opening the motor3\n");
		exit(1);
	}

	if (motor4 == NULL)
	{
		perror("An error occurred when opening the motor4\n");
		exit(1);
	}

	set_pwm_speed((long int)PWM_SPEED, (long int)PWM_SPEED,
				  (long int)PWM_SPEED, (long int)PWM_SPEED,
				  motor1, motor2, motor3, motor4);

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
	 * Init bluetooth
	 */

	session = bt_register_service();
	s = bt_server_register(&loc_addr);

	/*
	 * Init Kalman
	 */

	kalman_init(&kalman_x);
	kalman_init(&kalman_y);
	x_angle_cons = 0;
	y_angle_cons = 0;

	/*
	 * Init PID
	 */
	
	kp = 0;
	ki = 0;
	kd = 0;

	// Wait for sensor to stabilize
	sleep(1000);
	
	sensors_get_values(i2c_device, &sv);

	roll = atan(sv.accY / sqrt(sv.accX * sv.accX + sv.accZ * sv.accZ)) * RAD_TO_DEG;
	pitch = atan2(-sv.accX, sv.accZ) * RAD_TO_DEG;

	kalman_x.angle = roll;
	kalman_y.angle = pitch;

	t_start = clock();

	pthread_create(&tid, NULL, compute_kalman_filter, NULL);

	/*
	 * Main loop
	 */

	while (shut)
	{
		// waiting for bt client
		client = bt_server_initiate(s, &rem_addr);

		while ((end_b) && (end_w >= 0) && shut)
		{
			read(client, key, 1);
			//printf("%c\r", key[0]);

			/* 
			   Motors configuration

			   motor1		motor2



			   motor4		motor3
			*/

			switch (*key)
			{
			case 'U':
				read(client, data, sizeof(data));
				break;

			case 't':
				speed1 += INCREMENT;
				speed2 += INCREMENT;
				speed3 += INCREMENT;
				speed4 += INCREMENT;
				set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2,
							  motor3, motor4);
				break;

			case 'g':
				speed1 -= INCREMENT;
				speed2 -= INCREMENT;
				speed3 -= INCREMENT;
				speed4 -= INCREMENT;
				set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2,
							  motor3, motor4);
				break;

			case 'z':
				speed1 -= INCREMENT;
				speed2 -= INCREMENT;
				speed3 += INCREMENT;
				speed4 += INCREMENT;
				set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2,
							  motor3, motor4);
				break;

			case 's':
				speed1 += INCREMENT;
				speed2 += INCREMENT;
				speed3 -= INCREMENT;
				speed4 -= INCREMENT;
				set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2,
							  motor3, motor4);
				break;

			case 'q':
				speed1 -= INCREMENT;
				speed2 += INCREMENT;
				speed3 += INCREMENT;
				speed4 -= INCREMENT;
				set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2,
							  motor3, motor4);
				break;

			case 'd':
				speed1 += INCREMENT;
				speed2 -= INCREMENT;
				speed3 -= INCREMENT;
				speed4 += INCREMENT;
				set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2,
							  motor3, motor4);
				break;

			case 'k':
				end_b = 0;
				break;

			case 'l':
				shut = 0;
				break;

			case 'a':
				if (debug == 1)
					debug = 0;
				else
					debug = 1;
				break;
			
			case 0x01:
				read(client,coef, 5);
				if(coef[5] == 0x00)
				{
					coef[5] = 0x00;
					kp = atoi(coef);

					if(debug == 1)
					{
						sprintf(debug_inf, "Coef Kp set to: %d\n", kp);
						write(client, debug_inf, 21);
					}
				}
				else if(coef[5] == 0x01)
				{
					coef[5] = 0x00;
					ki = atoi(coef);
					if(debug == 1)
					{
						sprintf(debug_inf, "Coef Ki set to: %d\n", ki);
						write(client, debug_inf, 21);
					}
				}
				else if(coef[5] == 0x02)
				{
					coef[5] = 0x00;
					kd = atoi(coef);
					if(debug == 1)
					{
						sprintf(debug_inf, "Coef Kd set to: %d\n", kd);
						write(client, debug_inf, 21);
					}
				}
				break;
			}

			if (debug == 1)
			{
				sprintf(debug_inf, "Speed1: %ld\n", speed1);
				write(client, debug_inf, 19);
				sprintf(debug_inf, "Speed2: %ld\n", speed2);
				write(client, debug_inf, 19);
				sprintf(debug_inf, "Speed3: %ld\n", speed3);
				write(client, debug_inf, 19);
				sprintf(debug_inf, "Speed4: %ld\n", speed4);
				write(client, debug_inf, 19);
			}

			end_w = write(client, "ok", 2);

		}

		end_w = 0;
		end_b = 1;

		close(client);
		//printf("\nDisconnected\n");
	}

	set_pwm_speed((long int)PWM_SPEED, (long int)PWM_SPEED,
				  (long int)PWM_SPEED, (long int)PWM_SPEED, motor1, motor2,
				  motor3, motor4);

	fclose(motor1);
	fclose(motor2);
	fclose(motor3);
	fclose(motor4);

	bt_end_session(s, session);

	system("shutdown now\n\r");

	return 0;
}
