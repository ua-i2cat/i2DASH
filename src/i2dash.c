#include "i2dash.h"
#include "segment.h"
#include "debug.h"


i2DASHError i2dash_write(i2DASHContext *context, const char *buffer, int buffer_len)
{   
    int next_frame_number = context->frame_number + 1;

    if (context->frame_number == 0) {
        if (i2dash_segment_open(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_open");
            return i2DASH_ERROR;
        }
    }
    else if (next_frame_number % context->frames_per_segment == 0) {
        if (i2dash_segment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_close");
            return i2DASH_ERROR;
        }
        if (i2dash_segment_open(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_open");
            return i2DASH_ERROR;
        }
    }

    i2DASHError ret = i2dash_segment_write(context, buffer, buffer_len);
    if (ret == i2DASH_OK) {
        context->frame_number = next_frame_number;
    }
    else {
        i2dash_debug_err("i2dash_segment_write");
    }
    return ret;
}
