#ifndef IMU_DATA_H
#define IMU_DATA_H
//----------------------------------------------------------------------
#include <inttypes.h>

#include "gyroscope_data.h"
#include "accelerometer_data.h"

//----------------------------------------------------------------------
typedef struct _IMU_data
{
	Gyroscope_unit_data_t g;
	Accelerometer_unit_data_t a;
} IMU_data_t;
//----------------------------------------------------------------------
int IMU_data2json(char *json_str, size_t json_str_len, IMU_data_t *data);
//----------------------------------------------------------------------
#endif // IMU_DATA_H
