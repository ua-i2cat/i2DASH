/*
 *  DemuxerTest.cpp - Demuxer class test
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

#include "Demuxer.hh"
#include "Frame.hh"

using namespace std;

int main(int argc, char* argv[])
{
    AVCCFrame* videoFrame;
    AACFrame* audioFrame;
    Frame* frame;
    int gotFrame;
    Demuxer* demux = new Demuxer();

    demux->openInput(argv[1]);
    demux->findStreams();
    demux->dumpFormat();

    while (gotFrame >= 0){

        frame = demux->readFrame(gotFrame);

        if ((videoFrame = dynamic_cast<AVCCFrame*>(frame)) != NULL) {
            /*
            newSample = dashVideoSeg->addFrameToSample(videoFrame);
            
            if (newSample) {
                writeToDisk(dashVideoSeg->getSampleBuffer(), dashVideoSeg->getSampleLength, sampleName)
            }
            */
            std::cout << "VideoFrame buffer length: " << videoFrame->getLength() << std::endl;

            for (int i=0; i < videoFrame->getHLength(); i ++) {
                printf("%x ", videoFrame->getFrameHBuf()[i]);
            }

            printf("\n\n");
        }

        if ((audioFrame = dynamic_cast<AACFrame*>(frame)) != NULL) {
            /*
            newSample = dashAudioSeg->addFrameToSample(videoFrame);
            
            if (newSample) {
                writeToDisk(dashAudioSeg->getSampleBuffer(), dashAudioSeg->getSampleLength, sampleName)
            }
            */
            std::cout << "AudioFrame sample rate: " << audioFrame->getLength() << std::endl;
            std::cout << "AudioFrame: " << audioFrame->getSampleRate() << std::endl;
        }
    }

    delete demux;
    
    return 0;
} 
