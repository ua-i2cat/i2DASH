#include "context.h"


i2DASHError i2dash_context_initialize(i2DASHContext *context)
{
    context->segment_number = 0;
    context->fragment_number = 0;
    context->frame_number = 1;
    
    // Default values (segment duration in ms)
    context->segment_duration = 1000;
    context->frames_per_sample = 1;
    context->samples_per_fragment = 1;
    context->fragments_per_segment = 1;
    context->frame_rate = 24.0;
    context->file = NULL;
    context->sample = gf_isom_sample_new();

    return i2DASH_OK;
}

i2DASHError i2dash_context_update_frame_rate(i2DASHContext *context, float frame_rate)
{
    if (frame_rate <= 0.0) {
        return i2DASH_ERROR;
    }

    // TODO
    //return i2DASH_OK;
    return i2DASH_ERROR;
}

i2DASHError i2dash_context_reconfigure(i2DASHContext *context, int seg_nb, int frag_nb, int frame_nb, int seg_dur, float fps, char * dest_path, AVCodecContext * avccont)
{
    return i2DASH_ERROR;
}

