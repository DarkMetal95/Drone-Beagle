#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include "../include/libio.h"
#include "../include/libpwm.h"

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

	io_changemode(IO_MODE_RAW);

	fputs("PWM_SPEED", file);

	while (end)
	{
		while (!io_kbhit());
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
	io_changemode(IO_MODE_COOKED);

	return 0;
}
