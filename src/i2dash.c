#include "i2dash.h"
#include "segment.h"
#include "fragment.h"
#include "debug.h"


i2DASHError i2dash_write(i2DASHContext *context, const char *buffer, int buffer_len)
{   

    int next_frame_number = context->frame_number + 1;

    if (context->frame_number == 0) {
        if (i2dash_segment_open(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_open: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_open: OK");

        if(i2dash_fragment_open(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_fragment_open: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_fragment_open: OK");
    }
    else if (next_frame_number % context->frames_per_segment == 0) {
        if(i2dash_fragment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_fragment_close: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_fragment_close: OK");

        if (i2dash_segment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_close: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_close: OK");

        if (i2dash_segment_open(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_open: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_open: OK");

        if(i2dash_fragment_open(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_fragment_open: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_fragment_open: OK");
    }
   
    if(i2dash_fragment_write(context, buffer, buffer_len, 0, 0) != i2DASH_OK) {
        i2dash_debug_err("i2dash_fragment_write: KO");
        return i2DASH_ERROR;
    }
    i2dash_debug_msg("i2dash_fragment_write: OK");

    context->frame_number = next_frame_number;

    return i2DASH_OK;
}

