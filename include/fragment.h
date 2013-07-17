#ifndef _FRAGMENT_H_
#define _FRAGMENT_H_

#include <libavcodec/avcodec.h>
#include "context.h"
#include "sample.h"


i2DASHError i2dash_fragment_open();

i2DASHError i2dash_fragment_add_sample();

i2DASHError i2dash_fragment_close();

#endif

