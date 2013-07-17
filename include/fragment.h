#ifndef _FRAGMENT_H_
#define _FRAGMENT_H_

#include <libavcodec/avcodec.h>
#include "context.h"
#include "sample.h"

typedef struct {
	u32 track_number;
	u32 duration;
	u32 size;
	u32 description_index;
	u32 random_access;
    u16 default_degradation_priority;
    u8 default_padding;
} i2DASHFragmentDefaults;

i2DASHError i2dash_fragment_open(i2DASHContext *context, GF_ISOFile *file);

i2DASHError i2dash_fragment_add_sample();

i2DASHError i2dash_fragment_close();

#endif

