#include "sample.h"


i2DASHError i2dash_add_sample_buffer(i2DASHContext *context, uint8_t * buf, int buf_len)
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
    AVCodecContext *avcodec_ctx = context->avcodeccontext;
    uint8_t *buf =  NULL;
    printf("width %d, height %d\n", avcodec_ctx->width, avcodec_ctx->height);
    int buf_len = 9 * avcodec_ctx->width * avcodec_ctx->height + 10000;
    buf = (uint8_t *) av_malloc(buf_len);
    i2DASHError add_error;
    printf("buffer initialized\n");
    frame->pts = context->frame_number;
    printf("pts %d\n", frame->pts);
    
    int encoded_frame_size = avcodec_encode_video(avcodec_ctx, buf, buf_len, frame);
    printf("did encoded_frame_size");
    if(encoded_frame_size < 0){
        fprintf(stderr, "Error occured while encoding video frame.\n");
        return i2DASH_ERROR;
    }
    printf("encoded frame size: %d\n", encoded_frame_size);
    printf("buf_len: %d\n", buf_len);

    add_error = i2dash_add_sample_buffer(context, buf, buf_len);

    if (add_error != i2DASH_OK){
        return i2DASH_ERROR;
    }
    return i2DASH_OK;
}
