#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "../include/libbluetooth.h"
#include "../include/libio.h"
#include "../include/libpwm.h"

#define INCREMENT			1000

int main()
{
	int end_b = 1, s, client;
	char speed_c[] = "PWM_SPEED";
	long int speed = PWM_SPEED;
	char key = 0x00;
	FILE *motor1 = NULL;
	FILE *motor2 = NULL;
	FILE *motor3 = NULL;
	FILE *motor4 = NULL;
	sdp_session_t *session = NULL;
	sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

	motor1 = fopen(PWM_DUTY_FILE, "w");
	motor2 = fopen(PWM_DUTY_FILE, "w");
	motor3 = fopen(PWM_DUTY_FILE, "w");
	motor4 = fopen(PWM_DUTY_FILE, "w");

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

	session = bt_register_service();
	s = bt_server_register(&loc_addr);

	io_changemode(IO_MODE_RAW);

	fputs("PWM_SPEED", motor1);
	fputs("PWM_SPEED", motor2);
	fputs("PWM_SPEED", motor3);
	fputs("PWM_SPEED", motor4);

	while (end_b)
	{
		client = bt_server_initiate(s, &rem_addr);

		while (end_b)
		{
			read(s, (char *)&key, sizeof(key));

			if (key == 'z')
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
			else if (key == 's')
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
			else
				end_b = 0;
		}
		close(client);
	}

	bt_end_session(s, session);

	rewind(motor1);
	rewind(motor2);
	rewind(motor3);
	rewind(motor4);
	fputs("PWM_SPEED", motor1);
	fputs("PWM_SPEED", motor2);
	fputs("PWM_SPEED", motor3);
	fputs("PWM_SPEED", motor4);
	fclose(motor1);
	fclose(motor2);
	fclose(motor3);
	fclose(motor4);
	io_changemode(IO_MODE_COOKED);

	return 0;
}
