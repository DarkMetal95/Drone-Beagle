#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include "../include/libbluetooth.h"
#include "../include/libio.h"
#include "../include/libpwm.h"

#define INCREMENT			1000

int main()
{
	int end_b = 1, end_w = 0, shut = 1;
	char speed_c[] = "PWM_SPEED";
	long int speed = (long int) PWM_SPEED;
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

	sprintf(speed_c, "%ld", (long int) PWM_SPEED);
	fputs(speed_c, motor1);
	fputs(speed_c, motor2);
	fputs(speed_c, motor3);
	fputs(speed_c, motor4);

	while (shut)
	{
		bt_server_initiate();

		while((end_b) && (end_w >= 0))
		{
			read(client, key, 1);
			printf("%c\r", key[0]);
			if (*key == 'z')
			{
				rewind(motor1);
				rewind(motor2);
				rewind(motor3);
				rewind(motor4);
				speed += INCREMENT;
				sprintf(speed_c, "%ld", speed);
				fputs(speed_c, motor1);
				fputs(speed_c, motor2);
				fputs(speed_c, motor3);
				fputs(speed_c, motor4);
			}
			else if (*key == 's')
			{
				rewind(motor1);
				rewind(motor2);
				rewind(motor3);
				rewind(motor4);
				speed -= INCREMENT;
				sprintf(speed_c, "%ld", speed);
				fputs(speed_c, motor1);
				fputs(speed_c, motor2);
				fputs(speed_c, motor3);
				fputs(speed_c, motor4);
			}
			else if (*key == 'k')
			{
				end_b = 0;
			}
			else if (*key == 'l')
			{
				shut = 0;
			}

			end_w = write(client, speed_c, 10);
		}
		end_w = 0;
		end_b = 1;
		close(client);
	}

	rewind(motor1);
	rewind(motor2);
	rewind(motor3);
	rewind(motor4);
	sprintf(speed_c, "%ld", (long int) PWM_SPEED);
	speed = (long int) PWM_SPEED;
	fputs(speed_c, motor1);
	fputs(speed_c, motor2);
	fputs(speed_c, motor3);
	fputs(speed_c, motor4);

	fclose(motor1);
	fclose(motor2);
	fclose(motor3);
	fclose(motor4);

	end();

	io_changemode(IO_MODE_COOKED);

	system("shutdown now\n\r");

	return 0;
}
