#include "sample.h"


i2DASHError i2dash_add_sample_buffer(i2DASHContext *context, uint8_t * buf, int buf_len)
{    
    AVCodecContext * avcodec_ctx = context->avcodeccontext;
    printf("AVCodecContecxt loaded.\n");

    GF_BitStream * out_bs = gf_bs_new(NULL, 2 * buf_len, GF_BITSTREAM_WRITE);
    printf("Created new GF_BitStream with our data.\n");

    if(buf_len != 0){
        gf_bs_write_u32(out_bs, buf_len);
        gf_bs_write_data(out_bs, (const char*) buf, buf_len);
    }
    printf("Start: input data -> isomSample.\n");
    // TODO FIX: gf_bs_get_content
    gf_bs_get_content(out_bs, &context->sample->data,
                    &context->sample->dataLength);
    printf("Finish: input data -> isomSample.\n");

    context->sample->DTS = avcodec_ctx->coded_frame->pts;
    printf("DTS: %d", (int)context->sample->DTS);

    context->sample->IsRAP = avcodec_ctx->coded_frame->key_frame;
    printf("IsRAP: %d", context->sample->IsRAP);

    return i2DASH_OK;
}

// Won't work!
i2DASHError i2dash_add_sample_frame(i2DASHContext *context, AVFrame *frame)
{
    AVCodecContext *avcodec_ctx = context->avcodeccontext;
    AVPacket packet;
    i2DASHError add_error;
    // int encoded_frame_size;
    int packet_ok = 0;
    //uint8_t *buf =  NULL;
    printf("width %d, height %d\n", avcodec_ctx->width, avcodec_ctx->height);
    //int buf_len = 9 * avcodec_ctx->width * avcodec_ctx->height + 10000;
    //buf = (uint8_t *) av_malloc(buf_len);
    printf("buffer initialized\n");
    frame->pts = context->frame_number;
    printf("pts %d\n", (int)frame->pts);
    
    int error = avcodec_encode_video2(avcodec_ctx, &packet, frame, &packet_ok);
    // int encoded_frame_size = avcodec_encode_video(avcodec_ctx, buf, buf_len, frame);
    printf("did encoded_frame_size");
    if(error < 0){
        fprintf(stderr, "Error occured while encoding video frame.\n");
        return i2DASH_ERROR;
    }
    printf("packet_ok: %d\n", packet_ok);
    printf("packet size: %d\n", packet.size);

    add_error = i2dash_add_sample_buffer(context, packet.data, packet.size);

    if (add_error != i2DASH_OK){
        return i2DASH_ERROR;
    }
    return i2DASH_OK;
}
