#include "i2dash.h"
#include "segment.h"
#include "fragment.h"
#include "debug.h"


i2DASHError i2dash_write(i2DASHContext *context, const char *buffer, int buffer_len)
{   
    int next_frame_number = context->frame_number + 1;
    
    // init file with moov creation
    if (context->segment_number == 0 && context->frame_number == 0) {
        char segment_path[256];
        bzero(segment_path, 256);

        i2dash_debug_msg("Starting init segment");
        int ret = sprintf(segment_path, "%s_init.mp4",
                          (const char *)context->path);
        if (ret < 0) {
            i2dash_debug_err("init segment");
            return i2DASH_ERROR;
        }

        if (i2dash_segment_new(context, segment_path) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_new: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_segment_new: OK");

        if(i2dash_fragment_setup(context) != i2DASH_OK){
            i2dash_debug_err("i2dash_fragment_setup: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_fragment_setup: OK");
        i2dash_debug_msg("Init finished: %s\n", segment_path);

        i2dash_debug_msg("Starting segment %d", context->segment_number);
        i2dash_debug_msg("Starting fragment %d", context->fragment_number);
        int ret2 = sprintf(segment_path, "%s_%d.m4s",
                          (const char *)context->path,
                          context->segment_number);
        if (ret2 < 0) {
            i2dash_debug_err("segment: %d", context->segment_number);
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("start segment: %s", segment_path);

        // start segments
        if(i2dash_segment_start(context, segment_path) != i2DASH_OK){
            i2dash_debug_err("i2dash_segment_start: KO");
            return i2DASH_ERROR;
        }
        i2dash_debug_msg("i2dash_segment_start: OK");
        context->segment_number++;
        context->fragment_number++;
    }
    else if (next_frame_number % context->frames_per_segment == 0) {
        if(i2dash_fragment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_fragment_close: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_fragment_close: OK");

        if (i2dash_segment_close(context) != i2DASH_OK) {
            i2dash_debug_err("i2dash_segment_close: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_segment_close: OK\n");

        i2dash_debug_msg("finished fragment: %d", context->fragment_number-1);
        i2dash_debug_msg("finished segment: %d\n", context->segment_number-1);

        context->frame_number = 0;

        i2dash_debug_msg("Starting segment %d", context->segment_number);
        i2dash_debug_msg("Starting fragment %d", context->fragment_number);

        char segment_path[256];
        bzero(segment_path, 256);

        int ret = sprintf(segment_path, "%s_%d.m4s",
                          (const char *)context->path,
                          context->segment_number);
        if (ret < 0) {
            i2dash_debug_err("segment: %d", context->segment_number);
            return i2DASH_ERROR;
        }

        if(i2dash_segment_start(context, segment_path) != i2DASH_OK){
            i2dash_debug_err("i2dash_segment_start: KO");
            return i2DASH_ERROR;
        }
        //i2dash_debug_msg("i2dash_segment_start: OK");

        context->fragment_number++;
        context->segment_number++;
    }

    if(i2dash_fragment_write(context, buffer, buffer_len, 0, 0) != i2DASH_OK) {
        i2dash_debug_err("i2dash_fragment_write: KO");
        return i2DASH_ERROR;
    }
    //i2dash_debug_msg("i2dash_fragment_write: OK");

    i2dash_debug_msg("Written frame: %d\n", context->frame_number);
    context->frame_number++;

    return i2DASH_OK;
}

