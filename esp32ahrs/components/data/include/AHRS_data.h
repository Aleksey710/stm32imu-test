#ifndef AHRS_DATA_H
#define AHRS_DATA_H
//----------------------------------------------------------------------
#include <inttypes.h>

#include "IMU_data.h"
#include "magnetometer_data.h"

//----------------------------------------------------------------------
typedef struct _AHRS_data
{
	IMU_data_t imu;
	Magnetometer_unit_data_t m;
} AHRS_data_t;
//----------------------------------------------------------------------
int AHRS_data2json(char *json_str, size_t json_str_len, AHRS_data_t *data);
//----------------------------------------------------------------------
#endif // AHRS_DATA_H
