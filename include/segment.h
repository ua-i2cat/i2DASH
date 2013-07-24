#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "error.h"
#include "context.h"

i2DASHError i2dash_segment_new(i2DASHContext *context, char *segment_path);
i2DASHError i2dash_segment_start(i2DASHContext *context, char *segment_path);
i2DASHError i2dash_segment_write(i2DASHContext *context, const char *buffer, int buffer_len);
i2DASHError i2dash_segment_close(i2DASHContext *context);

#endif
