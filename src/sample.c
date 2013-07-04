#include "sample.h"


i2DASHError i2dash_add_sample_buffer(i2DASHContext *context, unsigned char * buf, u32 buf_len)
{    
    AVCodecContext * avcodec_ctx = context->avcodeccontext;

    GF_BitStream * out_bs = gf_bs_new(NULL, 2 * buf_len, GF_BITSTREAM_WRITE);

    if(buf_len != 0){
        gf_bs_write_u32(out_bs, buf_len);
        gf_bs_write_data(out_bs, (const char*) buf, buf_len);
    }

    gf_bs_get_content(out_bs, &context->sample->data,
                    &context->sample->dataLength);

    context->sample->DTS = avcodec_ctx->coded_frame->pts;
    context->sample->IsRAP = avcodec_ctx->coded_frame->key_frame;

    return i2DASH_OK;
}

i2DASHError i2dash_add_sample_frame(i2DASHContext *context, AVFrame *frame)
{
    AVCodecContext * avcodec_ctx = context->avcodeccontext;

    unsigned char * buf;
    u32 buf_len;

    int encoded_frame_size = avcodec_encode_video(avcodec_ctx, &buf, buf_len, frame);

    if(encoded_frame_size < 0){
        fprintf(stderr, "Error occured while encoding video frame.\n");
        return i2DASH_ERROR;
    }

    if (i2dash_add_sample_buffer(context, buf, buf_len) != i2DASH_OK){
        return i2DASH_ERROR;
    }

    return i2DASH_OK;
}
