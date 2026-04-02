#ifndef WS_MSG_RINGBUF_H
#define WS_MSG_RINGBUF_H
//----------------------------------------------------------------------
#include <inttypes.h>

#include "config.h"

//----------------------------------------------------------------------
#define WS_RINGBUF_MAX_DATA_SIZE 200
#define WS_MSG_POOL_SIZE 10
//----------------------------------------------------------------------
typedef struct
{
    char data[WS_RINGBUF_MAX_DATA_SIZE];
    size_t len;
} Ws_msg_data_t;
//----------------------------------------------------------------------
typedef struct
{
    int id;
    Ws_msg_data_t data;
} Ws_msg_t;

//----------------------------------------------------------------------
Ws_msg_t *alloc_ws_msg(void);
void free_ws_msg(Ws_msg_t *msg);
//----------------------------------------------------------------------
#endif // WS_MSG_RINGBUF_H
