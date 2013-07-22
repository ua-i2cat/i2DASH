#include "sample.h"
#include "debug.h"

#include <string.h>


i2DASHError i2dash_sample_add(i2DASHContext *context, const char *buf,
                              int buf_len, int dts, int key_frame)
{    
    GF_Err ret;
    
    context->sample = gf_isom_sample_new();
    assert(context->sample != NULL);
    
    GF_BitStream * out_bs = gf_bs_new(NULL, 2 * buf_len, GF_BITSTREAM_WRITE);

    if(buf_len != 0) {
        gf_bs_write_u32(out_bs, buf_len);
        gf_bs_write_data(out_bs, (const char*)buf, buf_len);
    }

    gf_bs_get_content(out_bs, &context->sample->data,
                      &context->sample->dataLength);

    int j;
    for(j=0;j<5;j++){
        i2dash_debug_msg("sample data: %c", context->sample->data[j]);
    }

    context->sample->DTS = dts;
    i2dash_debug_msg("context->sample->DTS: %d", (int)context->sample->DTS);
    context->sample->IsRAP = key_frame;
    i2dash_debug_msg("context->sample->IsRAP: %d", (int)context->sample->IsRAP);

    ret = gf_isom_fragment_add_sample(context->file, 1,
                    context->sample, 1, 1, 0, 0, 0);

    // TODO fix bad_param
    /*
        !movie->moof
        !(movie->FragmentsFlags & GF_ISOM_FRAG_WRITE_READY)
    */

    if (ret != GF_OK) {
        i2dash_debug_err("gf_isom_fragment_add_sample: %s",
                         gf_error_to_string(ret));
        return i2DASH_ERROR;
    }
    
    // gf_isom_sample_del(&context->sample);
    // i2dash_debug_msg("gf_isom_sample_del");
    
    return i2DASH_OK;
}

// Won't work!
i2DASHError i2dash_sample_add_frame(i2DASHContext *context, AVFrame *frame)
{
    AVCodecContext *avcodec_ctx = context->avcodeccontext;
    AVPacket packet;
    i2DASHError add_error;
    // int encoded_frame_size;
    int packet_ok = 0;
    //uint8_t *buf =  NULL;
    i2dash_debug_msg("width %d, height %d", avcodec_ctx->width, avcodec_ctx->height);
    //int buf_len = 9 * avcodec_ctx->width * avcodec_ctx->height + 10000;
    //buf = (uint8_t *) av_malloc(buf_len);
    i2dash_debug_msg("buffer initialized");
    frame->pts = context->frame_number;
    i2dash_debug_msg("pts %d", (int)frame->pts);
    
    int error = avcodec_encode_video2(avcodec_ctx, &packet, frame, &packet_ok);
    // int encoded_frame_size = avcodec_encode_video(avcodec_ctx, buf, buf_len, frame);
    i2dash_debug_msg("did encoded_frame_size");
    if(error < 0){
        i2dash_debug_err("Error occured while encoding video frame.");
        return i2DASH_ERROR;
    }
    i2dash_debug_msg("packet_ok: %d", packet_ok);
    i2dash_debug_msg("packet size: %d", packet.size);

    add_error = i2dash_sample_add(context, (const char*)packet.data, packet.size, 0, 0); // TODO

    if (add_error != i2DASH_OK){
        return i2DASH_ERROR;
    }
    return i2DASH_OK;
}
