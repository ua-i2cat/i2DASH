#include "segment.h"
#include "fragment.h"
#include "debug.h"
#include "error.h"
#include <string.h>


i2DASHError i2dash_segment_new(i2DASHContext *context)
{
    GF_Err err;

    u32 description_index;
    u32 timescale = context->frame_rate;

    char segment_path[256];
    bzero(segment_path, 256);

    int ret = sprintf(segment_path, "%s.mp4",
                      (const char *)context->path);
    if (ret < 0) {
        i2dash_debug_err("segment");
        return i2DASH_ERROR;
    }
    /*// set timescale
    err = gf_isom_set_timescale(context->file, timescale);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_set_timescale: %s",
                gf_error_to_string(ret));
        return i2DASH_ERROR;
    }*/

    u32 track = gf_isom_new_track(context->file, 1, GF_ISOM_MEDIA_VISUAL,
           timescale);
    if(!track) {
        i2dash_debug_err("gf_isom_new_track: %d", (int)track);
        return i2DASH_ERROR;
    }

    err = gf_isom_set_track_enabled(context->file, 1, 1);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_set_track_enabled: %s",
                gf_error_to_string(ret));
        return i2DASH_ERROR;
    }
    /*
    avccfg = gf_odf_avc_cfg_new();

    if (!avccfg) {
        i2dash_debug_err("Cannot create AVCConfig");
        return i2DASH_ERROR;
    }

    avccfg->configurationVersion = 1;
    */

    err = gf_isom_avc_config_new(context->file, 1, context->avccfg, NULL, NULL, 
                                    &description_index);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_avc_config_new: %s",
                        gf_error_to_string(ret));
        return i2DASH_ERROR;
    }

    gf_odf_avc_cfg_del(context->avccfg);

    err = gf_isom_avc_set_inband_config(context->file, track, 1);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_avc_set_inband_config: %s",
                        gf_error_to_string(ret));
        return i2DASH_ERROR;
    }

    return i2DASH_OK;
}

i2DASHError i2dash_segment_start(i2DASHContext *context)
{
    char segment_path[256];
    bzero(segment_path, 256);

    int ret = sprintf(segment_path, "%s.%d.m4s",
                      (const char *)context->path,
                      context->segment_number + 1);
    if (ret < 0) {
        i2dash_debug_err("segment open");
        return i2DASH_ERROR;
    }

    // GF_TRUE -> write on disk instead of memory
    GF_Err err = gf_isom_start_segment(context->file, segment_path, 1);
    if (err != GF_OK) {
        i2dash_debug_err("gf_isom_start_segment: %s", 
                            gf_error_to_string(ret));
        return i2DASH_ERROR;
    }

    return i2DASH_OK;
}

i2DASHError i2dash_segment_close(i2DASHContext *context)
{
    //GF_Err ret = GF_OK;

    GF_Err ret = gf_isom_close_segment(context->file, 0, 0, 0, 0, 0, 1,
                                context->segment_marker, NULL, NULL);
    if (ret != GF_OK) {
        i2dash_debug_err("gf_isom_close_segment: %s", 
                            gf_error_to_string(ret));
        return i2DASH_ERROR;
    }

    return i2DASH_OK;
}
