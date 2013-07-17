#include "i2dash.h"

i2DASHError i2dash_write_frame(i2DASHContext *context, const char *buffer, int buffer_len)
{   
    int next_frame_number = context->frame_number + 1;

    if (context->frame_number == 0) {
        if (i2dash_segment_start(context) != i2DASH_OK) {
            return i2DASH_ERROR;
        }
    }
    else if (next_frame_number % context->frames_per_segment == 0) {
        if (i2dash_segment_close(context) != i2DASH_OK) {
            return i2DASH_ERROR;
        };
        if (i2dash_segment_start(context) != i2DASH_OK) {
            return i2DASH_ERROR;
        }
    }

    i2DASHError ret = i2dash_segment_write_frame(context, buffer, buffer_len);
    
    return ret;
}
