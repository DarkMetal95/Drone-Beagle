#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

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
	int i = 0;
	int end = 1;
	char speed_c[] = "1160000";
	long int speed = 1160000;
	char key = 0x00;
	FILE *fichier = NULL;

	fichier = fopen("/sys/devices/ocp.3/pwm_test_P8_13.15/duty", "w");
	if (fichier == NULL)
	{
		printf("Erreur d'ouverture du fichier\n");
		return 0;
	}

	changemode(1);

	fputs("1160000", fichier);

	while (end)
	{
		while (!kbhit());
		key = getchar();

		if (key == 'z')
		{
			rewind(fichier);
			speed += 1000;
			sprintf(speed_c, "%d", speed);
			fputs(speed_c, fichier);
		}
		else if (key == 's')
		{
			rewind(fichier);
			speed -= 1000;
			sprintf(speed_c, "%d", speed);
			fputs(speed_c, fichier);
		}
		else
			end = 0;
	}

	rewind(fichier);
	fputs("1160000", fichier);
	fclose(fichier);
	changemode(0);
	return 0;
}
