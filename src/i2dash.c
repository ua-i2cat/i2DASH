#include "i2dash.h"
#include "segment.h"
#include "debug.h"


i2DASHError i2dash_write(i2DASHContext *context, const char *buffer, int buffer_len)
{   
    i2DASHError ret;

    int next_frame_number = context->frame_number + 1;

    /*
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
    */

    ret = i2dash_segment_open(context);
    if(ret == i2DASH_OK) {
        ret = i2dash_segment_write(context, buffer, buffer_len);
        if (ret == i2DASH_OK) {
            context->frame_number = next_frame_number;
        }
        else {
            i2dash_debug_err("i2dash_segment_write");
            return i2DASH_ERROR;
        }
    }
    else {
        i2dash_debug_err("i2dash_segment_open");
        return i2DASH_ERROR;
    }

    /*
    ret = i2dash_segment_write(context, buffer, buffer_len);
    if (ret == i2DASH_OK) {
        context->frame_number = next_frame_number;
    }
    else {
        i2dash_debug_err("i2dash_segment_write");
    }
    return ret;
    */

    return i2DASH_OK;
}

i2DASHError i2dash_close(i2DASHContext * context)
{
    if(context->file != NULL) {        
        GF_Err err = gf_isom_close(context->file);
        if(err != GF_OK) {
            i2dash_debug_err("gf_isom_close: %s", gf_error_to_string(err));
            return i2DASH_ERROR;
        }
    }

    gf_isom_sample_del(&context->sample);
    i2dash_debug_msg("gf_isom_sample_del");

    return i2DASH_OK;
}

/*

Theoretical body:
    
    i2dash_segment_open
    i2dash_fragment_open

    i2dash_sample_add

    i2dash_fragment_close
    i2dash_segment_close

*/

//i2dash_context_free