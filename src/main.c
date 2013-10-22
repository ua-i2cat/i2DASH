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
    if ((argc < 2 ) || (argc > 3)){
        printf("usage: %s <output> [<input>]\n", argv[0]);
        return -1;
    }
    char *input_path = NULL;
	if (argc == 3)
		input_path = argv[2];
    char *output_path = argv[1];
    i2DASHError err;
    i2DASHContext *context = NULL;
   
    AVFormatContext *pFormatCtx = NULL;
    int videoStream;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    AVPacket packet;

    AVDictionary *optionsDict = NULL;

	int i, count;

    // new i2dash context
    context = i2dash_context_new(output_path);
    if (context == NULL) {                    
        i2dash_debug_err("i2dash_context_new");
        return -1;
    }

    // Register all formats and codecs
    av_register_all();

	if (input_path != NULL) {
    	// Open video file
		if(avformat_open_input(&pFormatCtx, input_path, NULL, NULL)!=0) {
		    return -1; // Couldn't open file
		}
	   
		// Retrieve stream information
		if(avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		    return -1; // Couldn't find stream information
		}
		
		// Dump information about file onto standard error
		av_dump_format(pFormatCtx, 0, input_path, 0);

		// Find the first video stream
		videoStream=-1;

		for(i = 0; i < pFormatCtx-> nb_streams; i++) {
		    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
		        videoStream = i;
		        break;
		    }
		}

		if(videoStream==-1) {
		    return -1; // Didn't find a video stream
		}
		
		// Get a pointer to the codec context for the video stream
		pCodecCtx=pFormatCtx->streams[videoStream]->codec;

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
	}

	context->avcodec = avcodec_find_encoder(CODEC_ID_H264);
	if (context->avcodec == NULL) {
			fprintf(stderr, "Output video codec %d not found\n", CODEC_ID_H264);
			return -1;
		}
	context->avcodeccontext = avcodec_alloc_context3(context->avcodec);

	context->avcodeccontext->codec_id = context->avcodec->id;
	context->avcodeccontext->codec_type = AVMEDIA_TYPE_VIDEO;
	context->avcodeccontext->pix_fmt = PIX_FMT_YUV420P;
	
	if (input_path != NULL) {
		printf("bit_rate: %d width %d heigth %d time_base %d gop_size %d\n", pCodecCtx->bit_rate, pCodecCtx->width, pCodecCtx->height, pCodecCtx->time_base, pCodecCtx->gop_size);
		context->avcodeccontext->bit_rate = pCodecCtx->bit_rate;
		context->avcodeccontext->width = pCodecCtx->width;
		context->avcodeccontext->height = pCodecCtx->height;
		context->avcodeccontext->time_base = pCodecCtx->time_base;		
		context->avcodeccontext->gop_size = pCodecCtx->gop_size;
	}
	else {
		context->avcodeccontext->bit_rate = 8000000;
		context->avcodeccontext->width = 1280;
		context->avcodeccontext->height = 720;
		context->avcodeccontext->time_base = (AVRational){1, 24};
		context->avcodeccontext->gop_size = 24;
	}

	av_opt_set(context->avcodeccontext->priv_data, "preset", "ultrafast", 0);
	av_opt_set(context->avcodeccontext->priv_data, "tune", "zerolatency", 0);

	context->avcodeccontext->flags |= CODEC_FLAG_GLOBAL_HEADER;


	if (avcodec_open2(context->avcodeccontext, context->avcodec, NULL) < 0) {
		fprintf(stderr, "Cannot open output video codec\n");
	}
    
	err = i2dash_write_init(context);
	if (err != i2DASH_OK) {
		i2dash_debug_err("i2dash_write");
		return -1;
   	}

	
	if (input_path != NULL) {
		while(av_read_frame(pFormatCtx, &packet)>=0) {
		    i2dash_debug_msg("Reading frame %d", count);
		    if(packet.stream_index==videoStream) {
		        if(count++ > 1248) {
		            av_free_packet(&packet);
		            break;
		        }

		        err = i2dash_write_segment(context, (const char *)packet.data,
		                                       packet.size);
		        if (err != i2DASH_OK) {
		                i2dash_debug_err("i2dash_write");
		                return -1;
		        }
		    }
		    // Free the packet that was allocated by av_read_frame
		    av_free_packet(&packet);
		}
	 
		// Close the codec
		avcodec_close(pCodecCtx);

		// Close the video file
		avformat_close_input(&pFormatCtx);
	}
//	else
		

    i2dash_context_free(context);

    return 0;
}
