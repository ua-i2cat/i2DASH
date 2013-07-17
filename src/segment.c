#include "segment.h"
#include "error.h"


i2DASHError i2dash_segment_open(i2DASHContext *context)
{
    const char segment_path[256];
    int ret = sprintf(segment_path, (const char *)"%s.%d.m4s",
                      context->path, context->segment_number + 1);
    if (ret < 0) {
        return i2DASH_ERROR;
    }

    // GF_TRUE -> write on disk instead of memory
    GF_Err err = gf_isom_start_segment(context->file, segment_path, GF_TRUE);
    if (err != GF_OK) {
        return i2DASH_ERROR;
    }
    return i2DASH_OK;
}

i2DASHError i2dash_segment_write(i2DASHContext *context, const char *buffer, int buffer_len)
{
    return i2DASH_ERROR;
}

i2DASHError i2dash_segment_close(i2DASHContext *context)
{
    GF_Err err = GF_OK;
    err = gf_isom_close_segment(context->file, 0, 0, 0, 0, 0, 1,
                                context->segment_marker, NULL, NULL);
    if (err != GF_OK) {
        return i2DASH_ERROR;
    }

    return i2DASH_OK;
}
