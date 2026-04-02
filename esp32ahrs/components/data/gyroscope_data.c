#include "gyroscope_data.h"

#include <string.h>
#include <stdio.h>

#include "config.h"
//----------------------------------------------------------------------
int gyroscope_data2json(char *json_str, size_t json_str_len, Gyroscope_unit_data_t *data)
{
    return snprintf(json_str, json_str_len,
                    "{\"x\":" FMT_F ",\"y\":" FMT_F ",\"z\":" FMT_F "}",
                    (double)(data->x), (double)(data->y), (double)(data->z));
}
//----------------------------------------------------------------------
