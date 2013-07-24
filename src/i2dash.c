#include "i2dash.h"
#include "segment.h"
#include "fragment.h"
#include "debug.h"


i2DASHError i2dash_write(i2DASHContext *context, const char *buffer, int buffer_len)
{   

    // for more than one sample
    int next_frame_number = context->frame_number + 1;

    // if first segment and first frame
    if (context->segment_number == 0 && context->frame_number == 0) {
        if (i2dash_segment_new(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_newn: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_new: OK");

        if(i2dash_fragment_setup(context) != i2DASH_OK){
            i2dash_debug_err("i2dash_fragment_setup: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_fragment_setup: OK");

        // start segmentation
        if(i2dash_segment_start(context) != i2DASH_OK){
            i2dash_debug_err("i2dash_segment_start: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_start: OK");
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

        /*if (i2dash_segment_new(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_new: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_new: OK");*/

        if(i2dash_fragment_setup(context) != i2DASH_OK){
            i2dash_debug_err("i2dash_fragment_setup: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_fragment_setup: OK");

        if(i2dash_segment_start(context) != i2DASH_OK){
            i2dash_debug_err("i2dash_segment_start: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_start: OK");
    }

    // write fragment with samples
    if(i2dash_fragment_write(context, buffer, buffer_len, 0, 0) != i2DASH_OK) {
        i2dash_debug_err("i2dash_fragment_write: KO");
        return i2DASH_ERROR;
    }
    i2dash_debug_msg("i2dash_fragment_write: OK");
    context->frame_number++;
    // for more than one sample
    //context->frame_number = next_frame_number;
    // finalize fragmentation
    /*if(i2dash_fragment_close(context) != i2DASH_OK){
        i2dash_debug_err("i2dash_fragment_close: KO");
        return i2DASH_ERROR;
    }
    if(i2dash_segment_close(context) != i2DASH_OK){
        i2dash_debug_err("i2dash_segment_close: KO");
        return i2DASH_ERROR;
    }*/
    return i2DASH_OK;
}

