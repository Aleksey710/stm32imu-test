#ifndef GYROSCOPE_H
#define GYROSCOPE_H
//----------------------------------------------------------------------
#include <inttypes.h>

#include "common_data.h"

//----------------------------------------------------------------------
#ifdef GYROSCOPE_DATA_TYPE
typedef GYROSCOPE_DATA_TYPE Gyroscope_data_t;
#else
// typedef int16_t Gyroscope_data_t;
typedef float Gyroscope_data_t;
// typedef double Gyroscope_data_t;
#endif
//----------------------------------------------------------------------
typedef struct _gyroscope_unit_data
{
	Gyroscope_data_t x;
	Gyroscope_data_t y;
	Gyroscope_data_t z;
} Gyroscope_unit_data_t;
//----------------------------------------------------------------------
int gyroscope_data2json(char *json_str, size_t json_str_len, Gyroscope_unit_data_t *data);
//----------------------------------------------------------------------
#endif // GYROSCOPE_H
