#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <libavcodec/avcodec.h>
#include "context.h"

i2DASHError i2dash_sample_add_frame(i2DASHContext *context, AVFrame *frame);
i2DASHError i2dash_sample_add(i2DASHContext *context, const char * buf,
                              int buf_len, int dts, int key_frame);
i2DASHError i2dash_first_segment_create(i2DASHContext *context);

#endif
