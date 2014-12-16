/*
 *  Demuxer.hh - Demuxer class
 *  Copyright (C) 2014  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors:  David Cassany <david.cassany@i2cat.net>
 *            
 *            
 */

#ifndef _DEMUXER_HH
#define _DEMUXER_HH
extern "C" {
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
}

#include <string>
#include "Frame.hh"

#define N_OF_NAL_SIZE_BYTES_MINUS_ONE_POSITION 4
#define N_OF_NAL_SIZE_BYTES_MINUS_ONE_MASK 0x03
#define NAL_TYPE_MASK 0x1F
#define IDR_NAL_TYPE 5
#define SEI_NAL_TYPE 6

class Demuxer {
public:
    Demuxer(size_t vTime = 0, size_t aTime = 0);
    ~Demuxer();
    
    bool openInput(std::string filename);
    void closeInput();
    void dumpFormat();
    bool findStreams();
    Frame* const readFrame(int& gotFrame);
    
    float getFPS();
    size_t getWidth(){return videoStream->codec->width;};
    size_t getHeight(){return videoStream->codec->height;};
    size_t getVideoBitRate() {return videoStream->codec->bit_rate;};
    size_t getVideoTimeBase();
    size_t getVideoDuration(); //ms
    size_t getVideoSampleDuration();
    
    size_t getAudioSampleRate() {return audioStream->codec->sample_rate;};
    size_t getAudioChannels() {return audioStream->codec->channels;};
    size_t getAudioBitsPerSample();
    size_t getAudioBitRate() {return audioStream->codec->bit_rate;};
    size_t getAudioTimeBase();
    size_t getAudioDuration(); //ms
    size_t getAudioSampleDuration();
    
    bool hasVideo();
    bool hasAudio();

    unsigned char* getVideoExtraData();
    size_t getVideoExtraDataLength();

    unsigned char* getAudioExtraData();
    size_t getAudioExtraDataLength();
        
private:
    bool sourceExists(std::string filename);
    bool findVideoStream();
    bool findAudioStream();
    bool validVideoCodec();
    bool validAudioCodec();
    bool isIntra(unsigned char* data);
    size_t getNalSizeBytes(unsigned char* metadata);
          
private:
      
    AVFormatContext *fmtCtx;
    AVPacket pkt;
    AVStream *audioStream, *videoStream;
    
    int videoStreamIdx, audioStreamIdx;
    int framesCounter;
    bool isOpen;
       
    size_t vStartTime; //Video TimeBase Ticks
    size_t aStartTime; //Audio TimeBase Ticks
    
    AVCCFrame* const videoFrame;
    AACFrame* const audioFrame;

    size_t nalSizeBytes;
};

#endif
