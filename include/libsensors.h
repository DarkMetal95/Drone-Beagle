#ifndef SENSORS_H
#define SENSORS_H

/*
 * Defines
 */

/*
 * Structures
 */

typedef struct Sensors_values
{
	short gyroX;
	short gyroY;
	short gyroZ;
	short accX;
	short accY;
	short accZ;
} Sensors_values;

/*
 * Prototypes
 */

void sensors_get_values(int device, Sensors_values *values);

#endif
