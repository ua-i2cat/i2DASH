#include <stdio.h>
#include <stdbool.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "i2dash.h"
#include "context.h"
#include "sample.h"
#include "debug.h"


int main(int argc, char *argv[])
{	
	// TODO: add argc audio, video, both
    if ((argc < 5 ) || (argc > 7)){
        i2dash_debug_msg("usage: %s -o <output> -f <video/audio/both> [-i <input>]", argv[0]);
        return -1;
    }

    i2DASHError err;
    i2DASHContext *context = NULL;

	AVFormatContext *vFormatCtx = NULL;
	AVFormatContext *aFormatCtx = NULL;

    AVCodecContext *vCodecCtx = NULL;
    AVCodecContext *aCodecCtx = NULL;

    AVCodec *vCodec = NULL;
    AVCodec *aCodec = NULL;

    AVPacket packet;
    AVDictionary *optionsDict = NULL;

	int i = 1;
	int j, count, videoStream;

    char *input_path = NULL;
	char *output_path = argv[i+1];
	bool video = false;
	bool audio = false;
	bool both = false;

    while((i+1) < argc)
    {
		if(strcmp(argv[i], "-o") == 0) {
			i2dash_debug_msg("output path: %s", output_path);
			i +=2;
		} else if(strcmp(argv[i], "-f") == 0) {
		    i2dash_debug_msg("input kind: %s", argv[i+1]);
			if(strcmp(argv[i+1], "both") == 0) {
				both = true;
				audio = true;
				video = true;
				i += 2;
			} else if(strcmp(argv[i+1], "video") == 0) {
				video = true;
				i += 2;
			} else if(strcmp(argv[i+1], "audio") == 0) {
				audio = true;
				i += 2;
			} else {
				i2dash_debug_msg("usage: %s -o <output> -f <video/audio/both> [-i <input>]", argv[0]);
				return -1;
			}
		} else if(strcmp(argv[i+1], "-i") == 0) {
			input_path = argv[i+1];
			i2dash_debug_msg("input path: %s", argv[i+1]);
			i += 2;
		} else {
		i2dash_debug_msg("usage: %s -o <output> -f <video/audio/both> [-i <input>]", argv[0]);
		return -1;
		}
	}

    // new i2dash context
    context = i2dash_context_new(output_path);
    if (context == NULL) {                  
        i2dash_debug_err("i2dash_context_new");
        return -1;
    }

    if(both == true)
		context->both = true;
    
    // Register all formats and codecs
    av_register_all();

    if(video) {
    	i2dash_debug_msg("start video init");
	    if (input_path != NULL) {
	    	// Open video file
			if(avformat_open_input(&vFormatCtx, input_path, NULL, NULL)!=0) {
			    return -1; // Couldn't open file
			}
		   
			// Retrieve stream information
			if(avformat_find_stream_info(vFormatCtx, NULL) < 0) {
			    return -1; // Couldn't find stream information
			}
			
			// Dump information about file onto standard error
			av_dump_format(vFormatCtx, 0, input_path, 0);

			// Find the first video stream
			videoStream=-1;

			for(j = 0; j < vFormatCtx-> nb_streams; j++) {
			    if(vFormatCtx->streams[j]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
			        videoStream = j;
			        break;
			    }
			}

			if(videoStream==-1) {
			    return -1; // Didn't find a video stream
			}
			
			// Get a pointer to the codec context for the video stream
			vCodecCtx=vFormatCtx->streams[videoStream]->codec;

			// Find the decoder for the video stream
			vCodec=avcodec_find_decoder(vCodecCtx->codec_id);
			if(vCodec==NULL) {
			  i2dash_debug_err("Unsupported codec!");
			  return -1; // Codec not found
			}

			// Open codec
			if(avcodec_open2(vCodecCtx, vCodec, &optionsDict) < 0) {
			   i2dash_debug_err("Could not open codec");
			   return -1;
			}
		}

		context->vcodec = avcodec_find_encoder(CODEC_ID_H264);
		if (context->vcodec == NULL) {
				fprintf(stderr, "Output video codec %d not found\n", CODEC_ID_H264);
				return -1;
			}
		context->vcodeccontext = avcodec_alloc_context3(context->vcodec);

		context->vcodeccontext->codec_id = context->vcodec->id;
		context->vcodeccontext->codec_type = AVMEDIA_TYPE_VIDEO;
		context->vcodeccontext->pix_fmt = PIX_FMT_YUV420P;
		
		if (input_path != NULL) {
			context->vcodeccontext->bit_rate = vCodecCtx->bit_rate;
			context->vcodeccontext->width = vCodecCtx->width;
			context->vcodeccontext->height = vCodecCtx->height;
			context->vcodeccontext->time_base = vCodecCtx->time_base;		
			context->vcodeccontext->gop_size = vCodecCtx->gop_size;
		}
		else {
			context->vcodeccontext->bit_rate = 8000000;
			context->vcodeccontext->width = 1280;
			context->vcodeccontext->height = 720;
			context->vcodeccontext->time_base = (AVRational){1, 24};
			context->vcodeccontext->gop_size = 24;
		}

		av_opt_set(context->vcodeccontext->priv_data, "preset", "ultrafast", 0);
		av_opt_set(context->vcodeccontext->priv_data, "tune", "zerolatency", 0);

		context->vcodeccontext->flags |= CODEC_FLAG_GLOBAL_HEADER;

		if (avcodec_open2(context->vcodeccontext, context->vcodec, NULL) < 0) {
			i2dash_debug_err("Cannot open output video codec");
			return -1;
		}
	    
	    //Commented in order to test audio init generation
		err = i2dash_write_init_video(context);
		if (err != i2DASH_OK) {
			i2dash_debug_err("i2dash_write_init_video");
			return -1;
	   	}
    }

	if(audio) {
		//TODO all
   		i2dash_debug_msg("start audio init");
        if (input_path != NULL) {
        	i2dash_debug_msg("please, don't specify audio input by now");
        	return -1;
        }

        context->acodec = avcodec_find_encoder(CODEC_ID_AAC);
        if (context->acodec == NULL) {
        	//TODO
			i2dash_debug_err("Output audio codec not found");
			return -1;
		}

		context->acodeccontext = avcodec_alloc_context3(context->acodec);

		context->acodeccontext->codec_id = context->acodec->id;
		context->acodeccontext->codec_type = AVMEDIA_TYPE_AUDIO;
		context->acodeccontext->sample_fmt = AV_SAMPLE_FMT_S16;

		if(input_path == NULL) {
			context->acodeccontext->bit_rate = 256000;
			context->acodeccontext->sample_rate = 48000;
			context->acodeccontext->time_base  = (AVRational){1, 48000};
			context->acodeccontext->channels = 2;
			context->acodeccontext->channel_layout = AV_CH_LAYOUT_STEREO;
		}

		if (avcodec_open2(context->acodeccontext, context->acodec, NULL) < 0) {
			i2dash_debug_err("Cannot open output audio codec");
			return -1;
		}

		err = i2dash_write_init_audio(context);
   		if (err != i2DASH_OK) {
			i2dash_debug_err("i2dash_write_init_audio");
			return -1;
   		}
	}

	/*if (input_path != NULL) {
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
	}*/
//	else
		
    i2dash_context_free(context);

    return 0;
}
