/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * Demuxing example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode audio and video data.
 * @example demuxing_decoding.c
 */

#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <string.h>

int main (int argc, char **argv)
{
    int ret = 0;
    AVFormatContext *fmt_ctx = NULL;
    char *src_filename = NULL;

    int video_stream_idx = -1, audio_stream_idx = -1;
    AVPacket pkt;
    
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    
    src_filename = argv[1];

    av_register_all();

    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", src_filename);
        exit(1);
    }

    av_dump_format(fmt_ctx, 0, NULL, 0);
    
    video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    
    audio_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    if (video_stream_idx < 0 && audio_stream_idx < 0) {
        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
        ret = 1;
        goto end;
    }

    //while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        av_read_frame(fmt_ctx, &pkt);
        if (pkt.stream_index == video_stream_idx) {
            printf("video pkt size: %d idx: %d\n", pkt.size, video_stream_idx);
        } else if (pkt.stream_index == audio_stream_idx) {
            printf("audio pkt size: %d idx: %d\n", pkt.size, audio_stream_idx);
        } else {   
            printf("others\n");
        }
    //}

    printf("Demuxing succeeded.\n");
    
end:
    avformat_close_input(&fmt_ctx);

    return ret < 0;
}
