#include "../include/libkalman.h"

/*
 * Initializes the Kalman instance given as parameter
 */
void kalman_init(Kalman_instance *instance)
{
	instance->Q_angle = 0.001f;
	instance->Q_bias = 0.003f;
	instance->R_measure = 0.03f;

	instance->angle = 0.0f;
	instance->bias = 0.0f;

	instance->P[0][0] = 0.0f;
	instance->P[0][1] = 0.0f;
	instance->P[1][0] = 0.0f;
	instance->P[1][1] = 0.0f;
}

/*
 * Computes the new angle based on the data given as parameters.
 * The angle should be in degrees, the rate in degrees per second
 * and the delta time in seconds.
 */
void computeNewAngle(Kalman_instance *instance, float newAngle, float newRate, float dt)
{
	float S, K[2], y, P00_temp, P01_temp;

	instance->rate = newRate - instance->bias;
	instance->angle += dt * instance->rate;

	instance->P[0][0] += dt * (dt * instance->P[1][1] - instance->P[0][1] - instance->P[1][0] + instance->Q_angle);
	instance->P[0][1] -= dt * instance->P[1][1];
	instance->P[1][0] -= dt * instance->P[1][1];
	instance->P[1][1] += instance->Q_bias * dt;

	S = instance->P[0][0] + instance->R_measure;
	K[0] = instance->P[0][0] / S;
	K[1] = instance->P[1][0] / S;

	y = newAngle - instance->angle;
	instance->angle += K[0] * y;
	instance->bias += K[1] * y;

	P00_temp = instance->P[0][0];
	P01_temp = instance->P[0][1];

	instance->P[0][0] -= K[0] * P00_temp;
	instance->P[0][1] -= K[0] * P01_temp;
	instance->P[1][0] -= K[1] * P00_temp;
	instance->P[1][1] -= K[1] * P01_temp;
}
