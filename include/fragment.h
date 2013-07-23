#ifndef _FRAGMENT_H_
#define _FRAGMENT_H_

#include <libavcodec/avcodec.h>
#include "context.h"
#include "sample.h"


i2DASHError i2dash_fragment_setup(i2DASHContext *context);

i2DASHError i2dash_fragment_write(i2DASHContext *context, const char * buf,
                          int buf_len, int dts, int key_frame);

i2DASHError i2dash_fragment_close(i2DASHContext *context);

#endif

