#include "context.h"
#include <stdlib.h>


i2DASHContext *i2dash_context_new(const char *path)
{
    // GF_ISOM_WRITE_EDIT -> new file
    // 3rd param NULL -> will use the system's tmp folder
    GF_ISOFile *file = gf_isom_open(path, GF_ISOM_WRITE_EDIT, NULL);
    if (file == NULL) {
        return NULL;
    }
    
    i2DASHContext *context = malloc(sizeof(i2DASHContext));
    if (context == NULL) {
        return NULL;
    }

    context->path = (char *)path;

    context->segment_number = 0;
    context->fragment_number = 0;
    context->frame_number = 1;
    
    context->segment_marker = 0;

    context->segment_duration = 1000;

    context->frames_per_sample = 1;
    //context->samples_per_fragment = 24;
    //context->fragments_per_segment = 1;

    context->frames_per_fragment = 5;
    context->frames_per_segment = 5;

    context->fragment_dts = (uint64_t)0;

    context->frame_rate = 24.0;
    context->file = file;
    context->sample = NULL;

    return context;
}

i2DASHError i2dash_context_free(i2DASHContext *context)
{
    i2DASHError ret = i2DASH_OK;
    if (context->file != NULL) {
        if (gf_isom_close(context->file) != GF_OK) {
            ret = i2DASH_ERROR;
        }
    }

    if (context->sample != NULL) {
        gf_isom_sample_del(&context->sample);
    }
    
    /* free resources */
    free(context);

    return ret;
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

