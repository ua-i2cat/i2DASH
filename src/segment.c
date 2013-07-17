#include "segment.h"


i2DASHError i2dash_segment_open(i2DASHContext *context)
{
    if (context->segment != NULL) {
        return i2DASH_ERROR;
    }

    const char segment_path[256];
    int ret = sprintf(segment_path, "%s.%d.m4s", context->path,
                      context->segment_nb + 1);
    if (ret < 0) {
        return i2DASH_ERROR;
    }

    // GF_TRUE -> write on disk instead of memory
    i2DASHError err = gf_isom_start_segment(context->file, segment_path, GF_TRUE);
    if (err != GF_OK) {
        return i2DASH_ERROR;
    }
    return i2DASH_OK;
}

i2DASHError i2dash_segment_write(i2DASHContext *context, char *buffer, int buffer_len)
{
    
}

i2DASHError i2dash_segment_close(i2DASHContext *context)
{
    if (context->segment == NULL) {
        return i2DASH_ERROR;
    }

    GF_Err err = GF_OK;
    err = gf_isom_close_segment(context->file, 0, 0, 0, 0, 0, 1,
                                context->segment_marker, NULL, NULL);
    if (err != GF_OK) {
        return i2DASH_ERROR;
    }

    return i2DASH_OK;
}
