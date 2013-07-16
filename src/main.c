#include "i2dash.h"
#include "context.h"
#include "sample.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>


void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}

int main(int argc, char *argv[])
{
    i2DASHContext *context = NULL;
    i2DASHError init_error, add_error;
   
    AVFormatContext *pFormatCtx = NULL;
    int             i, videoStream;
    AVCodecContext  *pCodecCtx = NULL;
    AVCodec         *pCodec = NULL;
    AVPacket        packet;

    AVDictionary    *optionsDict = NULL;

    if(argc < 2) {
     printf("Please provide a movie file\n");
     return -1;
    }
    // Register all formats and codecs
    av_register_all();

    printf("opening input.\n");
    
    // Open video file
  if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
    return -1; // Couldn't open file

    printf("Retrieve stream information.\n");
    
    // Retrieve stream information
  if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    return -1; // Couldn't find stream information
    
    printf("Dump information.\n");
   
    // Dump information about file onto standard error
  av_dump_format(pFormatCtx, 0, argv[1], 0);

    // Find the first video stream
  videoStream=-1;
     printf("Find the first video stream.\n");
      for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      videoStream=i;
      break;
    }
  if(videoStream==-1)
    return -1; // Didn't find a video stream
    
    printf("Get a pointer...\n");
    
    // Get a pointer to the codec context for the video stream
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    printf("Find a decoder.\n");

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
      fprintf(stderr, "Unsupported codec!\n");
      return -1; // Codec not found
     }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
       return -1; // Could not open codec

    // Read frames and save first five frames to disk
    printf("Start reading frames.\n");
    
    context=malloc(sizeof(i2DASHContext));
    printf("Context sizeof %d\n", sizeof(i2DASHContext));
    
    init_error = i2dash_context_initiliaze(context);                    
    if(init_error != i2DASH_OK){
        printf("ERROR: i2dash_add_sample_frame.\n");
        return -1;
    }
    printf("Context initialized.\n");
                        
    printf("segment_number %d, fragment_number %d, frame_number %d, segment_duration %d,frames_per_sample %d, samples_per_fragment %d, fragments_per_segment %d, frame_rate %f,\n",
        context->segment_number, 
        context->fragment_number, context->frame_number,
        context->segment_duration,
        context->frames_per_sample,
        context->samples_per_fragment,
        context->fragments_per_segment,
        context->frame_rate
    );
                        
    context->avcodeccontext = pCodecCtx;
    printf("AVCodecContext loaded \n");
    
    i=0;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
        printf("Reading frame %d.\n", i);
        if(packet.stream_index==videoStream) {
            

            // Save the frame to disk
            if(++i <= 5) {
                break;
            }

            printf("START: sample %d\n", i);
            add_error = i2dash_add_sample_buffer(context, packet.data, packet.size);
            if(add_error != i2DASH_OK) {
                    printf("ERROR: i2dash_add_sample_frame.\n");
                    return -1;
            }
            printf("OK");
        }
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }
    

    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);

  return 0;
}
