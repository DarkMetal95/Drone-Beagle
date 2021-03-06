#ifndef KALMAN_H
#define KALMAN_H

/*
 * Defines
 */

#define PI					3.14159265
#define RAD_TO_DEG			57.2957786

/*
 * Structures
 */

typedef struct
{
	float Q_angle;
	float Q_bias;
	float R_measure;
	float angle;
	float bias;
	float rate;
	float P[2][2];

} Kalman_instance;

/*
 * Prototypes
 */

void kalman_init(Kalman_instance *instance);
void kalman_compute_new_angle(Kalman_instance *instance, float newAngle, float dt);

#endif
