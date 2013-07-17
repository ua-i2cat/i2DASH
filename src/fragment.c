#include "debug.h"
#include "fragment.h"


i2DASHError i2dash_fragment_open(i2DASHContext *context)
{
    GF_Err ret;
    file = context->file;
    
    ret = gf_isom_setup_track_fragment(file, 1, 1, 1, 0, 0, 0, 0);
    if(ret != GF_OK) {
        fprintf(stderr, "%s: gf_isom_avc_set_inband_config\n",
                                gf_error_to_string(ret));
        return i2DASH_ERROR;
    }

    ret = gf_isom_finalize_for_fragment(file, 1);
    if (ret != GF_OK) {
        fprintf(stderr, "%s: gf_isom_finalize_for_fragment\n",
                        gf_error_to_string(ret));
        return i2DASH_ERROR;
    }

    return i2DASH_OK;
}

i2DASHError i2dash_fragment_write(i2DASHContext *context, uint8_t * buf,
                          int buf_len, int dts, int key_frame)
{
    GF_Err ret;
    i2DASHError err;

    if(context->frame_number % context->samples_per_fragment == 0){
        ret = gf_isom_start_fragment(context->file, 1);

        if (ret != GF_OK) {
            fprintf(stderr, "%s: gf_isom_start_fragment\n",
                            gf_error_to_string(ret));
            return i2DASH_ERROR;
        }

        ret = gf_isom_set_traf_base_media_decode_time(context->file, 1,
                                                    context->fragment_dts);

        context->fragment_dts += context->samples_per_fragment;
    }

    err = i2dash_sample_add(&context, &buf, buf_len, dts, key_frame);

    if(err != i2DASH_OK) {
        // TODO define handle error
        printf("i2DASHError: i2dash_sample_add\n");
        return i2DASH_ERROR;
    }
    return i2DASH_OK;
}

i2DASHError i2dash_fragment_close()
{


}
