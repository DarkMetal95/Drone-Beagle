#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "../include/libbluetooth.h"
#include "../include/libpwm.h"
#include "../include/libsensors.h"
#include "../include/libi2c.h"
#include "../include/libkalman.h"

/*
 * Prootypes
 */

void motor_control();

/*
 * Global variables
 */

int i2c_device;

double roll, pitch;
double dt_time;
struct timeval t_start, t_end, dt;
Sensors_values sv;
Kalman_instance kalman_x, kalman_y;
int x_angle_cons, y_angle_cons, z_height_cons, z_t_height;
int kp, ki, kd;
int commande_x, commande_y;
long int speed1, speed2, speed3, speed4;
static pthread_mutex_t kalman_sync_mutex1;
static pthread_mutex_t kalman_sync_mutex2; 

FILE *motor1 = NULL, *motor2 = NULL, *motor3 = NULL, *motor4 = NULL;

void *compute_kalman_filter()
{
	while (1)
	{
		pthread_mutex_lock(&kalman_sync_mutex1);

		sensors_get_values(i2c_device, &sv);

		gettimeofday(&t_end, NULL);
		timersub(&t_end, &t_start, &dt);
		dt_time = dt.tv_sec+((double)dt.tv_usec/1000000);
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

		if (sv.gyroX < -180 || sv.gyroX > 180)
			sv.gyroX = kalman_x.angle;

		if (sv.gyroY < -180 || sv.gyroY > 180)
			sv.gyroY = kalman_y.angle;

		usleep(2000);

		pthread_mutex_unlock(&kalman_sync_mutex2);
	}

	return NULL;
}

void *PID()
{
	double errorx, sum_errorx = 0, prev_errorx = 0;
	double errory, sum_errory = 0, prev_errory = 0;

	while(1)
	{
		pthread_mutex_lock(&kalman_sync_mutex2);

		errorx = (double)x_angle_cons + kalman_x.angle;
		sum_errorx += errorx;
		commande_x = kp * errorx + ki * sum_errorx + kd * (errorx - prev_errorx);
		prev_errorx = errorx;

		errory = (double)y_angle_cons - kalman_y.angle;
		sum_errory += errory;
		commande_y = kp * errory + ki * sum_errory + kd * (errory - prev_errory);
		prev_errory = errory;

		motor_control();

		pthread_mutex_unlock(&kalman_sync_mutex1);
	}

	return NULL;
}

/* 
   Motors configuration

   motor1		motor2



   motor4		motor3
*/

void motor_control()
{
	speed1 += commande_y;
	speed4 += commande_y;
	speed2 -= commande_y;
	speed3 -= commande_y;

	speed1 -= commande_x;
	speed2 -= commande_x;
	speed3 += commande_x;
	speed4 += commande_x;

	speed1 += z_height_cons;
	speed2 += z_height_cons;
	speed3 += z_height_cons;
	speed4 += z_height_cons;
	
	z_t_height +=z_height_cons;
	
	if(speed1 < 1000000 || z_t_height < 1000)
		speed1 = 1000000;
	if(speed2 < 1000000 || z_t_height < 1000)
		speed2 = 1000000;
	if(speed3 < 1000000 || z_t_height < 1000)
		speed3 = 1000000;
	if(speed4 < 1000000 || z_t_height < 1000)
		speed4 = 1000000;
		
	if(speed1 > 2000000)
		speed1 = 2000000;
	if(speed2 > 2000000)
		speed2 = 2000000;
	if(speed3 > 2000000)
		speed3 = 2000000;
	if(speed4 > 2000000)
		speed4 = 2000000;

	set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2,
				  motor3, motor4);
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

	speed1 = (long int)PWM_SPEED;
	speed2 = (long int)PWM_SPEED;
	speed3 = (long int)PWM_SPEED;
	speed4 = (long int)PWM_SPEED;

	pthread_t tid;
	pthread_t t_pid;

	/*
	 * Init motors
	 */

	motor1 = fopen(PWM_DUTY_FILE1, "w");
	motor2 = fopen(PWM_DUTY_FILE2, "w");
	motor3 = fopen(PWM_DUTY_FILE3, "w");
	motor4 = fopen(PWM_DUTY_FILE4, "w");

	if (motor1 == NULL)
	{
		perror("An error occurred when opening the motor1");
		exit(1);
	}

	if (motor2 == NULL)
	{
		perror("An error occurred when opening the motor2");
		exit(1);
	}

	if (motor3 == NULL)
	{
		perror("An error occurred when opening the motor3");
		exit(1);
	}

	if (motor4 == NULL)
	{
		perror("An error occurred when opening the motor4");
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
		perror("An error occurred during i2c setup\n");
		exit(1);
	}

	if (mpu6050_init(i2c_device) != 0)
	{
		perror("An error occurred during MPU6050 setup\n");
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

	// Wait 100 ms for sensor to stabilize
	usleep(100000);
	
	sensors_get_values(i2c_device, &sv);

	roll = atan(sv.accY / sqrt(sv.accX * sv.accX + sv.accZ * sv.accZ)) * RAD_TO_DEG;
	pitch = atan2(-sv.accX, sv.accZ) * RAD_TO_DEG;

	kalman_x.angle = roll;
	kalman_y.angle = pitch;

	gettimeofday(&t_start, NULL);

	/*
	 * Init PID
	 */
	
	x_angle_cons = 0;
	y_angle_cons = 0;
	z_height_cons = 0;
	z_t_height = 0;
	kp = 1;
	ki = 1;
	kd = 1;

	/*
	 * Init threads
	 */

	kalman_sync_mutex1 = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	kalman_sync_mutex2 = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_unlock(&kalman_sync_mutex1);
	pthread_mutex_lock(&kalman_sync_mutex2);

	pthread_create(&tid, NULL, compute_kalman_filter, NULL);
	pthread_create(&t_pid, NULL, PID, NULL);

	/*
	 * Main loop
	 */

	while (shut)
	{
		client = bt_server_initiate(s, &rem_addr);

		while ((end_b) && (end_w >= 0) && shut)
		{
			read(client, key, 1);

			switch (*key)
			{
			case 'U':
				read(client, data, sizeof(data));

				if(data[1] <= 9)
					y_angle_cons = -((data[0]*4)*data[1])/9;
				else if(data[1] > 9  && data[1] <= 18)
					y_angle_cons =  ((data[0]*4)*(data[1]-18))/9;
				else if(data[1] > 18 && data[1] <= 27)
					y_angle_cons =  ((data[0]*4)*(data[1]-18))/9;
				else if(data[1] > 27 && data[1] <= 35)
					y_angle_cons = -((data[0]*4)*(data[1]-35))/9;

				if(data[1] <= 9)
					x_angle_cons = -((data[0]*4)*(data[1]-9 ))/9;
				else if(data[1] > 9  && data[1] <= 18)
					x_angle_cons = -((data[0]*4)*(data[1]-9 ))/9;
				else if(data[1] > 18 && data[1] <= 27)
					x_angle_cons =  ((data[0]*4)*(data[1]-27))/9;
				else if(data[1] > 27 && data[1] <= 35)
					x_angle_cons =  ((data[0]*4)*(data[1]-27))/9;

				if(data[3] <= 9)
					z_height_cons = -((data[2]*4)*(data[3]-9 ))/9;
				else if(data[3] > 9  && data[3] <= 18)
					z_height_cons = -((data[2]*4)*(data[3]-9 ))/9;
				else if(data[3] > 18 && data[3] <= 27)
					z_height_cons =  ((data[2]*4)*(data[3]-27))/9;
				else if(data[3] > 27 && data[3] <= 35)
					z_height_cons =  ((data[2]*4)*(data[3]-27))/9;
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
				if(coef[4] == 0x00)
				{
					coef[4] = 0x00;
					kp = atoi(coef);

					if(debug == 1)
					{
						sprintf(debug_inf, "Coef Kp set to: %d\n", kp);
						write(client, debug_inf, 21);
					}
				}
				else if(coef[4] == 0x01)
				{
					coef[4] = 0x00;
					ki = atoi(coef);
					if(debug == 1)
					{
						sprintf(debug_inf, "Coef Ki set to: %d\n", ki);
						write(client, debug_inf, 21);
					}
				}
				else if(coef[4] == 0x02)
				{
					coef[4] = 0x00;
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

		set_pwm_speed((long int)PWM_SPEED, (long int)PWM_SPEED, (long int)PWM_SPEED, (long int)PWM_SPEED, motor1, motor2, motor3, motor4);

		close(client);
	}

	i2c_close(i2c_device);

	fclose(motor1);
	fclose(motor2);
	fclose(motor3);
	fclose(motor4);

	bt_end_session(s, session);

	system("shutdown now\n\r");

	return 0;
}
