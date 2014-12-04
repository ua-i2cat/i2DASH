/*
 *  InitVideoTest.cpp - Dash init video test
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
 *            Marc Palau <marc.palau@i2cat.net>
 *            
 *            
 */

#include <string>
#include <iostream>

#include "include/i2libdash.h"
#include "Demuxer.hh"
#include "Frame.hh"
#include <assert.h>

#define SPS_POS_IN_METADATA 6

using namespace std;

int main(int argc, char* argv[])
{
    AVCCFrame* videoFrame;
    AACFrame* audioFrame;
    Frame* frame;
    int gotFrame;

    unsigned char* sps;
    unsigned char* pps;
    unsigned char* metadata;
    unsigned char* metadata2;
    unsigned char* metadata3;
    uint32_t spsSize;
    uint32_t ppsSize;
    uint32_t metadataSize;
    uint32_t metadata2Size;
    uint32_t metadata3Size;
    i2ctx* avContext;

    int buffIdx;
    uint8_t i2error;
    int segmentTime = 1;

    int initBufferMaxLen = 1024*1024; //1MB
    int initBufferLen = 0;
    int newInitBufferLen = 0;
    unsigned char* initBuffer = new unsigned char[initBufferMaxLen];
    unsigned char* newInitBuffer = new unsigned char[initBufferMaxLen];

    // METADATA
    metadataSize = 4;
    metadata = new unsigned char[metadataSize];
    // METADATA2
    metadata2Size = 2;
    metadata2 = new unsigned char[metadata2Size];
    // METADATA3
    metadata3Size = 1;
    metadata3 = new unsigned char[metadata3Size];

    Demuxer* demux = new Demuxer();

    demux->openInput(argv[1]);
    demux->findStreams();
    demux->dumpFormat();



    //while (gotFrame >= 0){
    while (newInitBufferLen == 0) {

        frame = demux->readFrame(gotFrame);

        if ((videoFrame = dynamic_cast<AVCCFrame*>(frame)) != NULL) {
            
            i2error = context_initializer(&avContext, VIDEO_TYPE);
            if (i2error == I2ERROR_MEDIA_TYPE) {
                printf ("Media type incorrect\n");
                break;
            }

            set_segment_duration(segmentTime, &avContext);
            buffIdx = 0;

            // METADATA
            metadata[0] = videoFrame->getFrameHBuf()[buffIdx];
            metadata[1] = videoFrame->getFrameHBuf()[buffIdx+1];
            metadata[2] = videoFrame->getFrameHBuf()[buffIdx+2];
            metadata[3] = videoFrame->getFrameHBuf()[buffIdx+3];

            buffIdx += metadataSize;
            
            metadata2[0] = videoFrame->getFrameHBuf()[buffIdx];
            metadata2[1] = videoFrame->getFrameHBuf()[buffIdx+1];

            buffIdx += metadata2Size;

            spsSize = (uint32_t)videoFrame->getFrameHBuf()[buffIdx] << 8  | (uint32_t)videoFrame->getFrameHBuf()[buffIdx+1];
            buffIdx += 2;

            sps = videoFrame->getFrameHBuf() + buffIdx;
            buffIdx += spsSize;

            metadata3[0] = videoFrame->getFrameHBuf()[buffIdx];
            buffIdx++;

            ppsSize = (uint32_t)videoFrame->getFrameHBuf()[buffIdx] << 8  | (uint32_t)videoFrame->getFrameHBuf()[buffIdx+1];
            buffIdx += 2;

            pps = videoFrame->getFrameHBuf() + buffIdx;


            initBufferLen = init_video_handler(metadata, metadataSize, metadata2, metadata2Size, 
                                               sps, &spsSize, metadata3, metadata3Size, pps, ppsSize, 
                                               initBuffer, &avContext);
            
            newInitBufferLen = new_init_video_handler(videoFrame->getFrameHBuf(), videoFrame->getHLength(), newInitBuffer, &avContext);


            assert (initBufferLen == newInitBufferLen);

            int n = memcmp (initBuffer, newInitBuffer, initBufferLen);

            assert (n == 0);
        }
    }

    delete demux;
    
    return 0;
} 
