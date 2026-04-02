#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H
//----------------------------------------------------------------------
#include <inttypes.h>

#include "common_data.h"

//----------------------------------------------------------------------
#ifdef ACCELEROMETER_DATA_TYPE
typedef ACCELEROMETER_DATA_TYPE Accelerometer_data_t;
#else
// typedef int16_t Accelerometer_data_t;
typedef float Accelerometer_data_t;
// typedef double Accelerometer_data_t;
#endif
//----------------------------------------------------------------------
typedef struct _accelerometer_unit_data
{
	Accelerometer_data_t x;
	Accelerometer_data_t y;
	Accelerometer_data_t z;
} Accelerometer_unit_data_t;
//----------------------------------------------------------------------
int accelerometer_data2json(char *json_str, size_t json_str_len, Accelerometer_unit_data_t *data);
//----------------------------------------------------------------------
#endif // ACCELEROMETER_H
