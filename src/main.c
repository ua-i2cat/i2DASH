#include <stdio.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "i2dash.h"
#include "context.h"
#include "sample.h"
#include "debug.h"


int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <input> <output>\n", argv[0]);
        return -1;
    }

    char *input_path = argv[1];
    char *output_path = argv[2];

    i2DASHContext *context = NULL;
   
    AVFormatContext *pFormatCtx = NULL;
    int videoStream;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    AVPacket packet;

    AVDictionary *optionsDict = NULL;

    // Register all formats and codecs
    av_register_all();

    i2dash_debug_msg("opening input");
    
    // Open video file
    if(avformat_open_input(&pFormatCtx, input_path, NULL, NULL)!=0) {
        return -1; // Couldn't open file
    }

    i2dash_debug_msg("Retrieve stream information");
    
    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        return -1; // Couldn't find stream information
    }
    
    i2dash_debug_msg("Dump information.");
    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, input_path, 0);

    // Find the first video stream
    videoStream=-1;
    i2dash_debug_msg("Find the first video stream.");
   
    int i;
    for(i = 0; i < pFormatCtx-> nb_streams; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

    if(videoStream==-1) {
        return -1; // Didn't find a video stream
    }
    
    i2dash_debug_msg("Get a pointer...");
    
    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    i2dash_debug_msg("Find a decoder.");

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
      i2dash_debug_err("Unsupported codec!");
      return -1; // Codec not found
    }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, &optionsDict) < 0) {
       i2dash_debug_err("Could not open codec");
       return -1;
    }

    // Read frames and save first five frames to disk
    i2dash_debug_msg("Start reading frames.");
    
    context = i2dash_context_new(output_path);
    
    if (context == NULL) {                    
        i2dash_debug_err("i2dash_context_new");
        return -1;
    }
    i2dash_debug_msg("Context initialized.");
                        
    i2dash_debug_msg("segment_number %d, fragment_number %d, frame_number %d, segment_duration %d,frames_per_sample %d, samples_per_fragment %d, fragments_per_segment %d, frame_rate %f",
        context->segment_number, 
        context->fragment_number, context->frame_number,
        context->segment_duration,
        context->frames_per_sample,
        context->samples_per_fragment,
        context->fragments_per_segment,
        context->frame_rate
    );
                        
    context->avcodeccontext = pCodecCtx;
    i2dash_debug_msg("AVCodecContext loaded");
    
    int count = 0;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        i2dash_debug_msg("Reading frame %d", count);
        if(packet.stream_index==videoStream) {
            if(count++ > 10) {
                av_free_packet(&packet);
                break;
            }

            i2dash_debug_msg("START: sample %d", count);
            i2DASHError err = i2dash_write(context, (const char *)packet.data,
                                           packet.size);
            if (err != i2DASH_OK) {
                    i2dash_debug_err("i2dash_write");
                    return -1;
            }
            i2dash_debug_msg("OK");
        }
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }
    
    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    i2dash_context_free(context);

    return 0;
}
