/*
 * Defines
 */

/*
 * Structures
 */

typedef struct Sensors_values
{
	int gyroX;
	int gyroY;
	int gyroZ;
	int accX;
	int accY;
	int accZ;
} Sensors_values;

/*
 * Prototypes
 */

void sensors_get_values(int device, Sensors_values *values);
