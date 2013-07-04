#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include "libav/include/libavcodec/avcodec.h"
#include "context.h"


i2DASHError i2dash_add_sample_frame(i2DASHContext *context, AVFrame *frame);
i2DASHError i2dash_add_sample_buffer(i2DASHContext *context, uint8_t * buf, int buf_len);

#endif
