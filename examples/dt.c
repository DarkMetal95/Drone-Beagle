#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int main(){

struct timeval t_start, t_end, dt;
gettimeofday(&t_start, NULL);
double dt_time;

	while(1){

		gettimeofday(&t_end, NULL);
		timersub(&t_end, &t_start, &dt);
		dt_time = dt.tv_sec+((double)dt.tv_usec/1000000);

		/*
		 * Voilà la variable à utiliser qui donne le temps en seconde !
		 *
		 * Le test avec 0,002 secondes, et ca marche !
		 */
		printf("%f\n", dt_time);


		gettimeofday(&t_start, NULL);

		usleep(2000);
	}

}