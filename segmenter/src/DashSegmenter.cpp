/*
 *  DashSegmenter.cpp - DashSegmenter class
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

int getSeqNumberFromPath(std::string filePath);
void constructVideoSegments(DashSegment*& segment, DashSegment*& initSegment, std::string filePath, int seqNumber, size_t segmentMaxLength);
void constructAudioSegments(DashSegment*& segment, DashSegment*& initSegment, std::string filePath, int seqNumber, size_t segmentMaxLength);

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
    int gotFrame = 0;
    int seqNumber = -1;

    std::string filePath = argv[1];
    seqNumber = getSeqNumberFromPath(filePath);


    demux = new Demuxer();
    vSeg = new DashVideoSegmenter();
    aSeg = new DashAudioSegmenter();
    
    demux->openInput(argv[1]);
    demux->findStreams();

    if (demux->hasVideo()) {
        if (!vSeg->init(demux->getVideoDuration(), demux->getVideoTimeBase(), 
                        demux->getVideoSampleDuration(), 
                        demux->getWidth(), demux->getHeight(), demux->getFPS())) {
            cerr << "Error initializing Video Segmenter" << endl;
            exit(1);
        }

        constructVideoSegments(vSegment, vInitSegment, filePath, seqNumber, vSeg->getMaxSegmentLength());

        if (!vSegment || !vInitSegment) {
            cerr << "Error constructing video DashSegment objects" << endl;
            exit(1);
        }

        if (!vSeg->generateInit(demux->getVideoExtraData(), demux->getVideoExtraDataLength(), vInitSegment)) {
            cerr << "Error constructing video init" << endl;
            exit(1);
        }
        
        vInitSegment->writeToDisk();
    }

    if (demux->hasAudio()) {
        if (!aSeg->init(demux->getAudioDuration(), demux->getAudioTimeBase(), demux->getAudioSampleDuration(), 
                        demux->getAudioChannels(), demux->getAudioSampleRate(), demux->getAudioBitsPerSample())) {
            cerr << "Error initializing Audio Segmenter" << endl;
            exit(1);
        }

        constructAudioSegments(aSegment, aInitSegment, filePath, seqNumber, aSeg->getMaxSegmentLength());

        if (!aSegment || !aInitSegment) {
            cerr << "Error constructing audio DashSegment objects" << endl;
            exit(1);
        }

        if (!aSeg->generateInit(demux->getAudioExtraData(), demux->getAudioExtraDataLength(), aInitSegment)) {
            cerr << "Error constructing audio DashSegment objects" << endl;
            exit(1);
        }
        
        aInitSegment->writeToDisk();
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

    if (demux->hasVideo() && vSeg->finishSegment(vSegment)) {
        vSegment->writeToDisk();
    }

    if (demux->hasAudio() && aSeg->finishSegment(aSegment)) {
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

int getSeqNumberFromPath(std::string filePath)
{
    int seqNumber = -1;

    size_t b = filePath.find_last_of("_");
    size_t e = filePath.find_last_of(".");

    std::string stringSequenceNumber = filePath.substr(b+1,e-b-1);
    seqNumber = stoi(stringSequenceNumber);

    return seqNumber;
}

void constructVideoSegments(DashSegment*& segment, DashSegment*& initSegment, std::string filePath, int seqNumber, size_t segmentMaxLength)
{
    size_t e = filePath.find_last_of(".");

    std::string vPath = filePath.substr(0,e) + ".m4v";
    std::string vInitPath = filePath.substr(0,e) + "_init.m4v";
    segment = new DashSegment(vPath, segmentMaxLength, seqNumber);
    initSegment = new DashSegment(vInitPath, segmentMaxLength, seqNumber);

    if (!segment || !initSegment) {
        std::cout << "ERROR IN CONSTRUCT" << std::endl;
    }

}

void constructAudioSegments(DashSegment*& segment, DashSegment*& initSegment, std::string filePath, int seqNumber, size_t segmentMaxLength)
{
    size_t e = filePath.find_last_of(".");

    std::string aPath = filePath.substr(0,e) + ".m4a";
    std::string aInitPath = filePath.substr(0,e) + "_init.m4a";
    segment = new DashSegment(aPath, segmentMaxLength, seqNumber);
    initSegment = new DashSegment(aInitPath, segmentMaxLength, seqNumber);

    if (!segment || !initSegment) {
        std::cout << "ERROR IN CONSTRUCT" << std::endl;
    }
}
