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
    Demuxer();
    ~Demuxer();
    
    bool openInput(string filename);
    void closeInput();
    void dumpFormat();
    bool findStreams();
    Frame* readFrame(int& gotFrame);
    
private:
    bool sourceExists(string filename);
    bool findVideoStream();
    bool findAudioStream();
    bool validVideoCodec();
    bool validAudioCodec();
          
private:
    AVFormatContext *fmtCtx;
    AVStream *videoStream, *audioStream;
    AVPacket pkt;
    
    int videoStreamIdx, audioStreamIdx;
    int framesCounter;
    bool isOpen;
    
    AVCCFrame* videoFrame;
    AACFrame* audioFrame;
};

#endif
