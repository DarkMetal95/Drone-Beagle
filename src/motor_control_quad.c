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

#define INCREMENT			1000

int main()
{
	int end_b = 1, end_w = 0, shut = 1;
	char speed_c1[] = "PWM_SPEED";
	char speed_c2[] = "PWM_SPEED";
	char speed_c3[] = "PWM_SPEED";
	char speed_c4[] = "PWM_SPEED";
	long int speed1 = (long int) PWM_SPEED;
	long int speed2 = (long int) PWM_SPEED;
	long int speed3 = (long int) PWM_SPEED;
	long int speed4 = (long int) PWM_SPEED;
	char key[1] = {0x00};
	FILE *motor1 = NULL;
	FILE *motor2 = NULL;
	FILE *motor3 = NULL;
	FILE *motor4 = NULL;

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

	bt_server_register();

	io_changemode(IO_MODE_RAW);

	sprintf(speed_c1, "%ld", (long int) PWM_SPEED);
	sprintf(speed_c2, "%ld", (long int) PWM_SPEED);
	sprintf(speed_c3, "%ld", (long int) PWM_SPEED);
	sprintf(speed_c4, "%ld", (long int) PWM_SPEED);
	fputs(speed_c1, motor1);
	fputs(speed_c2, motor2);
	fputs(speed_c3, motor3);
	fputs(speed_c4, motor4);

	while (shut)
	{
		bt_server_initiate();

		while((end_b) && (end_w >= 0))
		{
			read(client, key, 1);
			printf("%c\r", key[0]);

			rewind(motor1);
			rewind(motor2);
			rewind(motor3);
			rewind(motor4);

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
					sprintf(speed_c1, "%ld", speed1);
					sprintf(speed_c2, "%ld", speed2);
					sprintf(speed_c3, "%ld", speed3);
					sprintf(speed_c4, "%ld", speed4);
					fputs(speed_c1, motor1);
					fputs(speed_c2, motor2);
					fputs(speed_c3, motor3);
					fputs(speed_c4, motor4);
					break;

				case 'g':
					speed1 -= INCREMENT;
					speed2 -= INCREMENT;
					speed3 -= INCREMENT;
					speed4 -= INCREMENT;
					sprintf(speed_c1, "%ld", speed1);
					sprintf(speed_c2, "%ld", speed2);
					sprintf(speed_c3, "%ld", speed3);
					sprintf(speed_c4, "%ld", speed4);
					fputs(speed_c1, motor1);
					fputs(speed_c2, motor2);
					fputs(speed_c3, motor3);
					fputs(speed_c4, motor4);
					break;

				case 'z':
					speed1 -= INCREMENT;
					speed2 -= INCREMENT;
					speed3 += INCREMENT;
					speed4 += INCREMENT;
					sprintf(speed_c1, "%ld", speed1);
					sprintf(speed_c2, "%ld", speed2);
					sprintf(speed_c3, "%ld", speed3);
					sprintf(speed_c4, "%ld", speed4);
					fputs(speed_c1, motor1);
					fputs(speed_c2, motor2);
					fputs(speed_c3, motor3);
					fputs(speed_c4, motor4);
					break;

				case 's':
					speed1 += INCREMENT;
					speed2 += INCREMENT;
					speed3 -= INCREMENT;
					speed4 -= INCREMENT;
					sprintf(speed_c1, "%ld", speed1);
					sprintf(speed_c2, "%ld", speed2);
					sprintf(speed_c3, "%ld", speed3);
					sprintf(speed_c4, "%ld", speed4);
					fputs(speed_c1, motor1);
					fputs(speed_c2, motor2);
					fputs(speed_c3, motor3);
					fputs(speed_c4, motor4);
					break;

				case 'q':
					speed1 -= INCREMENT;
					speed2 += INCREMENT;
					speed3 += INCREMENT;
					speed4 -= INCREMENT;
					sprintf(speed_c1, "%ld", speed1);
					sprintf(speed_c2, "%ld", speed2);
					sprintf(speed_c3, "%ld", speed3);
					sprintf(speed_c4, "%ld", speed4);
					fputs(speed_c1, motor1);
					fputs(speed_c2, motor2);
					fputs(speed_c3, motor3);
					fputs(speed_c4, motor4);
					break;

				case 'd':
					speed1 += INCREMENT;
					speed2 -= INCREMENT;
					speed3 -= INCREMENT;
					speed4 += INCREMENT;
					sprintf(speed_c1, "%ld", speed1);
					sprintf(speed_c2, "%ld", speed2);
					sprintf(speed_c3, "%ld", speed3);
					sprintf(speed_c4, "%ld", speed4);
					fputs(speed_c1, motor1);
					fputs(speed_c2, motor2);
					fputs(speed_c3, motor3);
					fputs(speed_c4, motor4);
					break;

				case 'k':
					end_b = 0;
					break;

				case 'l':
					shut = 0;
					break;
			}

			write(client, speed_c1, 10);
			write(client, speed_c2, 10);
			write(client, speed_c3, 10);
			end_w = write(client, speed_c4, 10);
		}
		end_w = 0;
		end_b = 1;
		close(client);
	}

	rewind(motor1);
	rewind(motor2);
	rewind(motor3);
	rewind(motor4);
	sprintf(speed_c1, "%ld", (long int) PWM_SPEED);
	sprintf(speed_c2, "%ld", (long int) PWM_SPEED);
	sprintf(speed_c3, "%ld", (long int) PWM_SPEED);
	sprintf(speed_c4, "%ld", (long int) PWM_SPEED);
	fputs(speed_c1, motor1);
	fputs(speed_c2, motor2);
	fputs(speed_c3, motor3);
	fputs(speed_c4, motor4);

	fclose(motor1);
	fclose(motor2);
	fclose(motor3);
	fclose(motor4);

	end();

	io_changemode(IO_MODE_COOKED);

	system("shutdown now\n\r");

	return 0;
}
