/*
 * Defines
 */

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
void computeNewAngle(Kalman_instance *instance, float newAngle, float newRate, float dt);
