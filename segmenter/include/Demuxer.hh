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

using namespace std;

class Demuxer {
public:
    Demuxer(uint64_t vTime = 0, uint64_t aTime = 0);
    ~Demuxer();
    
    bool openInput(string filename);
    void closeInput();
    void dumpFormat();
    bool findStreams();
    Frame* const readFrame(int& gotFrame);
    
    float getFPS(){return fps;};
    size_t getWidth(){return width;};
    size_t getHeight(){return height;};
    uint32_t getVideoBitRate() {return videoBitRate;};
    
    uint32_t getAudioSampleRate() {return sampleRate;};
    uint32_t getAudioChannels() {return channels;};
    uint32_t getAudioBitsPerSample() {return bitsPerSample;};
    uint32_t getAudioBitRate() {return audioBitRate;};
    
    bool hasVideo();
    bool hasAudio();

    unsigned char* getVideoExtraData();
    size_t getVideoExtraDataLength();

    unsigned char* getAudioExtraData();
    size_t getAudioExtraDataLength();
        
private:
    bool sourceExists(string filename);
    bool findVideoStream();
    bool findAudioStream();
    bool validVideoCodec();
    bool validAudioCodec();
          
private:
    AVFormatContext *fmtCtx;
    AVPacket pkt;
    
    int videoStreamIdx, audioStreamIdx;
    int framesCounter;
    bool isOpen;
    
    uint32_t videoBitRate;
    uint32_t audioBitRate;

    float fps;
    size_t width;
    size_t height;
    
    size_t channels;
    size_t sampleRate;
    size_t bitsPerSample;
    
    uint64_t vStartTime;
    uint64_t aStartTime;
    
    AVCCFrame* const videoFrame;
    AACFrame* const audioFrame;
};

#endif
