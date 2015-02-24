#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#define PWM_DUTY_FILE		"/sys/devices/ocp.3/pwm_test_P8_13.15/duty"
#define PWM_SPEED			1160000

void changemode(int dir)
{
	static struct termios oldt, newt;

	if (dir == 1)
	{
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	}
	else
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit(void)
{
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);
}

int main()
{
	int end = 1;
	char speed_c[] = "PWM_SPEED";
	long int speed = PWM_SPEED;
	char key = 0x00;
	FILE *file = NULL;

	file = fopen(PWM_DUTY_FILE, "w");

	if (file == NULL)
	{
		perror("An error occurred when opening the file\n");
		exit(1);
	}

	changemode(1);

	fputs("PWM_SPEED", file);

	while (end)
	{
		while (!kbhit());
		key = getchar();

		if (key == 'z')
		{
			rewind(file);
			speed += 1000;
			sprintf(speed_c, "%ld", speed);
			fputs(speed_c, file);
		}
		else if (key == 's')
		{
			rewind(file);
			speed -= 1000;
			sprintf(speed_c, "%ld", speed);
			fputs(speed_c, file);
		}
		else
			end = 0;
	}

	rewind(file);
	fputs("PWM_SPEED", file);
	fclose(file);
	changemode(0);

	return 0;
}
