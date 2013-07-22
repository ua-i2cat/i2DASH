#include "debug.h"
#include "fragment.h"


i2DASHError i2dash_fragment_open(i2DASHContext *context)
{
    GF_Err ret;
    GF_ISOFile *file = context->file;
    
    ret = gf_isom_setup_track_fragment(file, 1, 1, 1, 0, 0, 0, 0);
    if(ret != GF_OK) {
        i2dash_debug_err("gf_isom_avc_set_inband_config: %s",
                                gf_error_to_string(ret));
        return i2DASH_ERROR;
    }

    ret = gf_isom_finalize_for_fragment(file, 1);
    if (ret != GF_OK) {
        i2dash_debug_err("gf_isom_finalize_for_fragment: %s",
                        gf_error_to_string(ret));
        return i2DASH_ERROR;
    }
    return i2DASH_OK;
}

i2DASHError i2dash_fragment_write(i2DASHContext *context, const char * buf,
                          int buf_len, int dts, int key_frame)
{
    GF_Err ret;

    if(context->frame_number % context->frames_per_fragment == 0){
        ret = gf_isom_start_fragment(context->file, GF_TRUE);
        if (ret != GF_OK) {
            i2dash_debug_err("gf_isom_start_fragment: %s",
                            gf_error_to_string(ret));
            return i2DASH_ERROR;
        }

        ret = gf_isom_set_traf_base_media_decode_time(context->file, 1,
                                                    context->fragment_dts);
        if (ret != GF_OK) {
            i2dash_debug_err("gf_isom_set_traf_base_media_decode_time: %s",
                            gf_error_to_string(ret));
            return i2DASH_ERROR;
        }

        context->fragment_dts += context->frames_per_fragment;
    }

    if(i2dash_sample_add(context, buf, buf_len, 0, 0) != i2DASH_OK) {
        i2dash_debug_err("i2dash_sample_add: KO");
        return i2DASH_ERROR;
    }
    i2dash_debug_msg("i2dash_sample_add: OK");
    
    return i2DASH_OK;
}

i2DASHError i2dash_fragment_close(i2DASHContext *context)
{
    GF_Err ret;

    if(context->frame_number % context->frames_per_fragment ==
                                context->frames_per_fragment - 1) {
        ret = gf_isom_flush_fragments(context->file, 1);
        if(ret != GF_OK) {
            i2dash_debug_err("gf_isom_flush_fragments: %s",
                         gf_error_to_string(ret));
            return i2DASH_ERROR;
        }
    }
    return i2DASH_OK;
}
