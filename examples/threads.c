#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t kalman_sync_mutex1;
pthread_mutex_t kalman_sync_mutex2; 

void *func1()
{
	while(1)
	{
		pthread_mutex_lock(&kalman_sync_mutex1);
		printf("Thread 1 running\n");
		sleep(1);
		pthread_mutex_unlock(&kalman_sync_mutex2);
	}
	return NULL;
}

void *func2()
{
	while(1)
	{
		pthread_mutex_lock(&kalman_sync_mutex2);
		printf("Thread 2 running\n");
		sleep(1);
		pthread_mutex_unlock(&kalman_sync_mutex1);
	}
	return NULL;
}

int main()
{
	pthread_t t_func1, t_func2;

	kalman_sync_mutex1 = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	kalman_sync_mutex2 = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&kalman_sync_mutex2);
	pthread_mutex_unlock(&kalman_sync_mutex1);

	pthread_create(&t_func1, NULL, func1, NULL);
	pthread_create(&t_func2, NULL, func2, NULL);

	while(1);
	
	return 0;
}