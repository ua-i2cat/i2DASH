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

int getSeqNumberFromPath(std::string filePath);
void constructVideoSegments(DashSegment*& segment, DashSegment*& initSegment, std::string filePath, int seqNumber, size_t segmentMaxLength);
void constructAudioSegments(DashSegment*& segment, DashSegment*& initSegment, std::string filePath, int seqNumber, size_t segmentMaxLength);

int main(int argc, char* argv[])
{
    Demuxer* demux = NULL;
    DashVideoSegmenter* vSeg = NULL;
    DashAudioSegmenter* aSeg = NULL;
    DashSegment* vInitSegment = NULL;
    DashSegment* aInitSegment = NULL;

    std::string filePath = argv[1];

    demux = new Demuxer();
    vSeg = new DashVideoSegmenter();
    aSeg = new DashAudioSegmenter();
    
    std::ofstream vHeaderFile("testData/DashVideoSegmenterTest_input_data", std::ofstream::binary);
    std::ofstream aHeaderFile("testData/DashAudioSegmenterTest_input_data", std::ofstream::binary);

    demux->openInput(argv[1]);
    demux->findStreams();

    if (demux->hasVideo()) {
        std::cout << "Video duration: " << demux->getVideoDuration() << std::endl;
        std::cout << "Video time base: " << demux->getVideoTimeBase() << std::endl;
        std::cout << "getVideoSampleDuration: " << demux->getVideoSampleDuration() << std::endl;
        std::cout << "getWidth: " << demux->getWidth() << std::endl;
        std::cout << "getHeight: " << demux->getHeight() << std::endl;
        std::cout << "getFPS: " << demux->getFPS() << std::endl;
        
        if (!vSeg->init(demux->getVideoDuration(), demux->getVideoTimeBase(), 
                        demux->getVideoSampleDuration(), 
                        demux->getWidth(), demux->getHeight(), demux->getFPS())) {
            std::cerr << "Error initializing Video Segmenter" << std::endl;
            exit(1);
        }

        vInitSegment = new DashSegment("testData/DashVideoSegmenterTest_init_model.m4v", vSeg->getMaxSegmentLength(), 0);

        if (vSeg->generateInit(demux->getVideoExtraData(), demux->getVideoExtraDataLength(), vInitSegment)) {
            vInitSegment->writeToDisk();
            vHeaderFile.write((char*)demux->getVideoExtraData(), demux->getVideoExtraDataLength());
        }
    }

    if (demux->hasAudio()) {
        std::cout << "getAudioDuration: " << demux->getAudioDuration() << std::endl;
        std::cout << "getAudioTimeBase: " << demux->getAudioTimeBase() << std::endl;
        std::cout << "getAudioSampleDuration: " << demux->getAudioSampleDuration() << std::endl;
        std::cout << "getAudioChannels: " << demux->getAudioChannels() << std::endl;
        std::cout << "getAudioSampleRate: " << demux->getAudioSampleRate() << std::endl;
        std::cout << "getAudioBitsPerSample: " << demux->getAudioBitsPerSample() << std::endl;

        if (!aSeg->init(demux->getAudioDuration(), demux->getAudioTimeBase(), demux->getAudioSampleDuration(), 
                        demux->getAudioChannels(), demux->getAudioSampleRate(), demux->getAudioBitsPerSample())) {
            std::cerr << "Error initializing Audio Segmenter" << std::endl;
            exit(1);
        }

        aInitSegment = new DashSegment("testData/DashAudioSegmenterTest_init_model.m4a", aSeg->getMaxSegmentLength(), 0);

        if (aSeg->generateInit(demux->getAudioExtraData(), demux->getAudioExtraDataLength(), aInitSegment)) {
            aInitSegment->writeToDisk();
            aHeaderFile.write((char*)demux->getAudioExtraData(), demux->getAudioExtraDataLength());
        }
    }

    vHeaderFile.close();
    aHeaderFile.close();

    delete demux;
    delete vSeg;
    delete aSeg;
    
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
