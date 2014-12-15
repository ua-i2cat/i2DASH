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
#include <fstream>

#include "Demuxer.hh"
#include "Frame.hh"
#include "DashVideoSegmenter.hh"
#include "DashAudioSegmenter.hh"
#include "DashSegment.hh"

using namespace std;

int main(int argc, char* argv[])
{
    Demuxer* demux = NULL;
    DashVideoSegmenter* vSeg = NULL;
    DashAudioSegmenter* aSeg = NULL;
    DashSegment* vSegment = NULL;
    DashSegment* aSegment = NULL;
    DashSegment* vInitSegment = NULL;
    DashSegment* aInitSegment = NULL;

    AVCCFrame* videoFrame;
    AACFrame* audioFrame;
    Frame* frame;
    int gotFrame;

    std::string filePath = argv[1];
    size_t b = filePath.find_last_of("_");
    size_t e = filePath.find_last_of(".");

    std::string stringSequenceNumber = filePath.substr(b+1,e-b-1);
    int seqNumber = stoi(stringSequenceNumber);

    std::cout << "seqNumber: " << seqNumber << std::endl;
 
    std::string vPath = filePath.substr(0,e) + ".m4v";
    std::string aPath = filePath.substr(0,e) + ".m4a";
    std::string vInitPath = filePath.substr(0,e) + "_init.m4v";
    std::string aInitPath = filePath.substr(0,e) + "_init.m4a";

    demux = new Demuxer();
    vSeg = new DashVideoSegmenter();
    aSeg = new DashAudioSegmenter();
    vSegment = new DashSegment(vPath, vSeg->getMaxSegmentLength(), seqNumber);
    aSegment = new DashSegment(aPath, aSeg->getMaxSegmentLength(), seqNumber);
    vInitSegment = new DashSegment(vInitPath, vSeg->getMaxSegmentLength(), seqNumber);
    aInitSegment = new DashSegment(aInitPath, aSeg->getMaxSegmentLength(), seqNumber);

    demux->openInput(argv[1]);
    demux->findStreams();
    // demux->dumpFormat();

    if (demux->hasVideo()) {
        if (!vSeg->init(demux->getDuration(), demux->getVideoTimeBase(), demux->getWidth(), demux->getHeight(), demux->getFPS())) {
            cout << "Error initializing Video Segmenter" << endl;
            exit(1);
        }

        if (vSeg->generateInit(demux->getVideoExtraData(), demux->getVideoExtraDataLength(), vInitSegment)) {
            vInitSegment->writeToDisk();
        }
    }

    if (demux->hasAudio()) {
        if (!aSeg->init(demux->getDuration(), demux->getAudioTimeBase(), demux->getAudioChannels(), demux->getAudioSampleRate(), demux->getAudioBitsPerSample())) {
            cout << "Error initializing Audio Segmenter" << endl;
            exit(1);
        }

        if (aSeg->generateInit(demux->getAudioExtraData(), demux->getAudioExtraDataLength(), aInitSegment)) {
            aInitSegment->writeToDisk();
        }
    }


    while (gotFrame >= 0){

        frame = demux->readFrame(gotFrame);

        if ((videoFrame = dynamic_cast<AVCCFrame*>(frame)) != NULL) {
            
            if (vSeg->addToSegment(videoFrame, vSegment)) {
                vSegment->writeToDisk();
            }
        }

        if ((audioFrame = dynamic_cast<AACFrame*>(frame)) != NULL) {

            if (aSeg->addToSegment(audioFrame, aSegment)) {
                aSegment->writeToDisk();
            }
        }
    }

    if (vSeg->finishSegment(vSegment)) {
        vSegment->writeToDisk();
    }

    if (aSeg->finishSegment(aSegment)) {
        aSegment->writeToDisk();
    }

    delete demux;
    delete vSeg;
    delete aSeg;
    delete vSegment;
    delete aSegment;
    delete vInitSegment;
    delete aInitSegment;
    
    return 0;
} 