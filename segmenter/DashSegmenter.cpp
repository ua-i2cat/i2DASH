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
    size_t e = filePath.find_first_of(".");

    std::string vPath = filePath.substr(0,e) + ".m4v";
    std::string aPath = filePath.substr(0,e) + ".m4a";
    std::string vInitPath = filePath.substr(0,e) + "_init.m4v";
    std::string aInitPath = filePath.substr(0,e) + "_init.m4a";

    demux = new Demuxer();
    vSeg = new DashVideoSegmenter();
    aSeg = new DashAudioSegmenter();
    vSegment = new DashSegment(vPath);
    aSegment = new DashSegment(aPath);
    vInitSegment = new DashSegment(vInitPath);
    aInitSegment = new DashSegment(aInitPath);

    demux->openInput(argv[1]);
    demux->findStreams();
    demux->dumpFormat();

    if (!vSeg->init()) {
        cout << "Error initializing Video Segmenter" << endl;
        exit(1);
    }

    if (!aSeg->init()) {
        cout << "Error initializing Audio Segmenter" << endl;
        exit(1);
    }

    while (gotFrame >= 0){
    // while (!initDone || !segmentDone) {

        frame = demux->readFrame(gotFrame);

        if ((videoFrame = dynamic_cast<AVCCFrame*>(frame)) != NULL) {
            
            if (vSeg->generateInit(videoFrame, vInitSegment)) {
                vInitSegment->writeToDisk();
            }

            if (vSeg->addToSegment(videoFrame, vSegment)) {
                vSegment->writeToDisk();
            }
        }

        if ((audioFrame = dynamic_cast<AACFrame*>(frame)) != NULL) {

            aSeg->setSampleRate(audioFrame->getSampleRate());

            if (aSeg->generateInit(audioFrame, aInitSegment)) {
                aInitSegment->writeToDisk();
            }

            if (aSeg->addToSegment(audioFrame, aSegment)) {
                aSegment->writeToDisk();
            }
        }
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