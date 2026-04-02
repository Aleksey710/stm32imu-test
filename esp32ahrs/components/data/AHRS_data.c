#include "AHRS_data.h"

#include <string.h>
#include <stdio.h>

#include "config.h"
#include "IMU_data.h"
#include "magnetometer_data.h"
//----------------------------------------------------------------------
int AHRS_data2json(char *json_str, size_t json_str_len, AHRS_data_t *data)
{
#define BUF_STR_LEN 50

	char imu_json_str[BUF_STR_LEN];
	IMU_data2json(imu_json_str, BUF_STR_LEN, &(data->imu));

	char m_json_str[BUF_STR_LEN];
	magnetometer_data2json(m_json_str, BUF_STR_LEN, &(data->m));

	return snprintf(json_str, json_str_len,
					"{\"imu\":%s,\"m\":%s}",
					imu_json_str, m_json_str);
}
//----------------------------------------------------------------------
