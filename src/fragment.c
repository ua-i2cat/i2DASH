#include "debug.h"
#include "fragment.h"

i2DASHError i2dash_fragment_open(i2DASHContext *context, GF_ISOFile *file)
{
    GF_Err ret;
    file = context->file;
    samples_per_fragment = context->samples_per_fragment;

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
