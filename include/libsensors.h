/*
 * Defines
 */

/*
 * Structures
 */

typedef struct Sensors_values
{
	float gyroX;
	float gyroY;
	float gyroZ;
	float accX;
	float accY;
	float accZ;
} Sensors_values;

/*
 * Prototypes
 */

void sensors_get_values(int device, Sensors_values *values);
