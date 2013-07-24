#include "i2dash.h"
#include "segment.h"
#include "fragment.h"
#include "debug.h"


i2DASHError i2dash_write(i2DASHContext *context, const char *buffer, int buffer_len)
{   

    // for more than one sample
    int next_frame_number = context->frame_number + 1;

    // TODO consider relation segment/fragment number

    // if first segment and first frame
    if (context->segment_number == 0 && context->frame_number == 0) {
        char segment_path[256];
        bzero(segment_path, 256);

        int ret = sprintf(segment_path, "%s_init.mp4",
                          (const char *)context->path);
        if (ret < 0) {
            i2dash_debug_err("segment init");
            return i2DASH_ERROR;
        }

        //context->segment_path = segment_path;

        if (i2dash_segment_new(context, segment_path) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_new: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_new: OK");

        if(i2dash_fragment_setup(context) != i2DASH_OK){
            i2dash_debug_err("i2dash_fragment_setup: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_fragment_setup: OK");

        // start segmentation
        if(i2dash_segment_start(context, segment_path) != i2DASH_OK){
            i2dash_debug_err("i2dash_segment_start: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_start: OK");

        context->segment_number++;

    }
    else if (next_frame_number % context->frames_per_segment == 0) {
        i2dash_debug_msg("finishing first segment");
        if(i2dash_fragment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_fragment_close: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_fragment_close: OK");

        if (i2dash_segment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_close: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_close: OK\n");

        context->frame_number = 0;

        i2dash_debug_msg("start new segment: %d", context->segment_number);

        char segment_path[256];
        bzero(segment_path, 256);

        int ret = sprintf(segment_path, "%s_%d.m4s",
                          (const char *)context->path,
                          context->segment_number);
        if (ret < 0) {
            i2dash_debug_err("segment: %d", context->segment_number);
            return i2DASH_ERROR;
        }
        //context->segment_path = segment_path;
        // start segmentation
        if(i2dash_segment_start(context, segment_path) != i2DASH_OK){
            i2dash_debug_err("i2dash_segment_start: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_start: OK");
        
        context->segment_number++;
    }

    // write fragment with samples
    if(i2dash_fragment_write(context, buffer, buffer_len, 0, 0) != i2DASH_OK) {
        i2dash_debug_err("i2dash_fragment_write: KO");
        return i2DASH_ERROR;
    }
    i2dash_debug_msg("i2dash_fragment_write: OK");

    i2dash_debug_msg("\n finish frame: %d \n", context->frame_number);
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

