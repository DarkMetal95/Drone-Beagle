#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include "../include/libbluetooth.h"
#include "../include/libio.h"
#include "../include/libpwm.h"
#include "../include/libsensors.h"
#include "../include/libi2c.h"
#include "../include/libkalman.h"

#define INCREMENT			1000

int main()
{
	sdp_session_t *session = NULL;
	sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	int i = 0, i2c_device;
	int s, client;
	int debug = 0;

	int end_b = 1, end_w = 0, shut = 1;
	long int speed1 = (long int) PWM_SPEED;
	long int speed2 = (long int) PWM_SPEED;
	long int speed3 = (long int) PWM_SPEED;
	long int speed4 = (long int) PWM_SPEED;
	char key[1] = {0x00};
	FILE *motor1 = NULL;
	FILE *motor2 = NULL;
	FILE *motor3 = NULL;
	FILE *motor4 = NULL;
	char speed_c1[] = "PWM_SPEED";
	char speed_c2[] = "PWM_SPEED";
	char speed_c3[] = "PWM_SPEED";
	char speed_c4[] = "PWM_SPEED";

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

	i2c_device = i2c_setup();
	session = bt_register_service();
	s = bt_server_register(&loc_addr);

	io_changemode(IO_MODE_RAW);

	set_pwm_speed((long int) PWM_SPEED, (long int) PWM_SPEED, (long int) PWM_SPEED, (long int) PWM_SPEED, motor1, motor2, motor3, motor4);

	while (shut)
	{
		client = bt_server_initiate(s, &rem_addr);

		while((end_b) && (end_w >= 0))
		{
			read(client, key, 1);
			printf("%c\r", key[0]);

			/*
			Motors configuration

			motor1   	  motor2



			motor4  	  motor3	
			*/

			switch(*key)
			{
				case 't':
					speed1 += INCREMENT;
					speed2 += INCREMENT;
					speed3 += INCREMENT;
					speed4 += INCREMENT;
					set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2, motor3, motor4);
					break;

				case 'g':
					speed1 -= INCREMENT;
					speed2 -= INCREMENT;
					speed3 -= INCREMENT;
					speed4 -= INCREMENT;
					set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2, motor3, motor4);
					break;

				case 'z':
					speed1 -= INCREMENT;
					speed2 -= INCREMENT;
					speed3 += INCREMENT;
					speed4 += INCREMENT;
					set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2, motor3, motor4);
					break;

				case 's':
					speed1 += INCREMENT;
					speed2 += INCREMENT;
					speed3 -= INCREMENT;
					speed4 -= INCREMENT;
					set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2, motor3, motor4);
					break;

				case 'q':
					speed1 -= INCREMENT;
					speed2 += INCREMENT;
					speed3 += INCREMENT;
					speed4 -= INCREMENT;
					set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2, motor3, motor4);
					break;

				case 'd':
					speed1 += INCREMENT;
					speed2 -= INCREMENT;
					speed3 -= INCREMENT;
					speed4 += INCREMENT;
					set_pwm_speed(speed1, speed2, speed3, speed4, motor1, motor2, motor3, motor4);
					break;

				case 'k':
					end_b = 0;
					break;

				case 'l':
					shut = 0;
					break;

				case 'a':
					if(debug == 1)
						debug = 0;
					else 
						debug = 1;
					break;
			}

			if(debug == 1){
				write(client, speed_c1, 10);
				write(client, speed_c2, 10);
				write(client, speed_c3, 10);
				end_w = write(client, speed_c4, 10);
			}
			else
				end_w = write(client, "ok", 2);
			
		}
		end_w = 1;
		end_b = 1;
		close(client);
		printf("\nDisconnected\n");
	}

	set_pwm_speed((long int) PWM_SPEED, (long int) PWM_SPEED, (long int) PWM_SPEED, (long int) PWM_SPEED, motor1, motor2, motor3, motor4);

	fclose(motor1);
	fclose(motor2);
	fclose(motor3);
	fclose(motor4);

	bt_end_session(s, session);

	end();

	io_changemode(IO_MODE_COOKED);

	system("shutdown now\n\r");

	return 0;
}

