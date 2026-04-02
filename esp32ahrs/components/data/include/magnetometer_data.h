#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H
//----------------------------------------------------------------------
#include <inttypes.h>

#include "common_data.h"

//----------------------------------------------------------------------
#ifdef MAGNETOMETER_DATA_TYPE
typedef MAGNETOMETER_DATA_TYPE Magnetometer_data_t;
#else
// typedef int16_t Magnetometer_data_t;
typedef float Magnetometer_data_t;
// typedef double Magnetometer_data_t;
#endif
//----------------------------------------------------------------------
typedef struct _magnetometer_unit_data
{
	Magnetometer_data_t x;
	Magnetometer_data_t y;
	Magnetometer_data_t z;
} Magnetometer_unit_data_t;
//----------------------------------------------------------------------
int magnetometer_data2json(char *json_str, size_t json_str_len, Magnetometer_unit_data_t *data);
//----------------------------------------------------------------------
#endif // MAGNETOMETER_H
