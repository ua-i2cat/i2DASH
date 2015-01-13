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
 *            
 *            
 */

#include <string>
#include <iostream>
#include <cpptest.h>
#include <algorithm>

#include "Demuxer.hh"
#include "Frame.hh"

#define BIG_START_TIME 4294967296 // 2^32

class DemuxerSuite : public Test::Suite
{
public:
    DemuxerSuite()
    {
        TEST_ADD(DemuxerSuite::constructorTest)
        TEST_ADD(DemuxerSuite::openInput)
        TEST_ADD(DemuxerSuite::dumpFormat)
        TEST_ADD(DemuxerSuite::findStreams)
        TEST_ADD(DemuxerSuite::readFrame)
        TEST_ADD(DemuxerSuite::closeInput)
        TEST_ADD(DemuxerSuite::destructorTest)
        
        TEST_ADD(DemuxerSuite::alternateConstructorTest)
        TEST_ADD(DemuxerSuite::openInput)
        TEST_ADD(DemuxerSuite::dumpFormat)
        TEST_ADD(DemuxerSuite::findStreams)
        TEST_ADD(DemuxerSuite::readFrame)
        TEST_ADD(DemuxerSuite::closeInput)
        TEST_ADD(DemuxerSuite::destructorTest)
    }
    
private:
    uint64_t startTime;
    Demuxer* demux = NULL;
    std::string filePath = "testData/DemuxerTest_input_data.mp4";
    void constructorTest();
    void correctParams();
    void destructorTest();
    void openInput();
    void closeInput();
    void dumpFormat();
    void findStreams();
    void readFrame();
    void alternateConstructorTest();
};


void DemuxerSuite::constructorTest()
{
    demux = new Demuxer();
    TEST_ASSERT(demux != NULL);
    startTime = 0;
}

void DemuxerSuite::alternateConstructorTest()
{
    demux = new Demuxer(BIG_START_TIME, BIG_START_TIME);
    TEST_ASSERT(demux != NULL);
    startTime = BIG_START_TIME;
}

void DemuxerSuite::destructorTest()
{
    TEST_ASSERT(demux != NULL);
    delete demux;
}

void DemuxerSuite::openInput()
{
    TEST_ASSERT(demux != NULL);
    TEST_ASSERT(!demux->hasVideo());
    TEST_ASSERT(!demux->hasAudio());
    TEST_ASSERT(demux->openInput(filePath));
    //TODO: redirect cerr
    TEST_ASSERT(!demux->openInput(filePath));
    TEST_ASSERT(!demux->hasVideo());
    TEST_ASSERT(!demux->hasAudio());
}

void DemuxerSuite::closeInput()
{
    TEST_ASSERT(demux != NULL);
    demux->closeInput();
    //TODO: redirect cerr
    demux->closeInput();
}

void DemuxerSuite::dumpFormat()
{
    TEST_ASSERT(demux != NULL);
    TEST_ASSERT(!demux->openInput(filePath));
    //TODO: assert format is correct
    demux->dumpFormat();
}

void DemuxerSuite::findStreams()
{
    TEST_ASSERT(demux != NULL);
    TEST_ASSERT(demux->findStreams());
    if (demux->hasVideo()){
        TEST_ASSERT(demux->getVideoBitRate() > 0);
        TEST_ASSERT(demux->getFPS() > 0);
        TEST_ASSERT(demux->getWidth() > 0);
        TEST_ASSERT(demux->getHeight() > 0);
        TEST_ASSERT(demux->getVideoDuration() > 0);
    } 
    if (demux->hasAudio()){
        TEST_ASSERT(demux->getAudioBitRate() > 0);
        TEST_ASSERT(demux->getAudioSampleRate() > 0);
        TEST_ASSERT(demux->getAudioBitsPerSample() > 0);
        TEST_ASSERT(demux->getAudioChannels() > 0);
        TEST_ASSERT(demux->getAudioDuration() > 0);
    } 
}

void DemuxerSuite::readFrame()
{
    AVCCFrame* vFrame;
    AACFrame* aFrame;
    Frame* frame;
    int gotFrame;
    bool videoFrame = false;
    bool audioFrame = false;
    size_t aDuration;
    size_t vDuration;
    
    TEST_ASSERT(demux != NULL);
    TEST_ASSERT(!demux->openInput(filePath));
    TEST_ASSERT(demux->findStreams());
    aDuration = demux->getAudioDuration();
    TEST_ASSERT(aDuration > 0);
    vDuration = demux->getVideoDuration();
    TEST_ASSERT(vDuration > 0);
    
    do {
        frame = demux->readFrame(gotFrame);
        if (dynamic_cast<AVCCFrame*>(frame)){
            vFrame = dynamic_cast<AVCCFrame*>(frame);
            videoFrame = true;
            TEST_ASSERT(vFrame->getDataBuffer() != NULL);
            TEST_ASSERT(vFrame->getDataLength() > 0);
            TEST_ASSERT(vFrame->getDuration() > 0);
        } else if (dynamic_cast<AACFrame*>(frame)){
            aFrame = dynamic_cast<AACFrame*>(frame);
            audioFrame = true;
            TEST_ASSERT(aFrame->getDataBuffer() != NULL);
            TEST_ASSERT(aFrame->getDataLength() > 0);
            TEST_ASSERT(aFrame->getDuration() > 0);
        }
    } while(gotFrame >= 0);
    
    if (demux->hasVideo()){
        TEST_ASSERT(videoFrame == true);
        TEST_ASSERT(vFrame != NULL);
        TEST_ASSERT(vFrame->getPresentationTime() > startTime);
    } 
    
    if (demux->hasAudio()){
        TEST_ASSERT(audioFrame == true);
        TEST_ASSERT(aFrame != NULL);
        TEST_ASSERT(aFrame->getPresentationTime() > startTime);
    }
    
    std::cout << " vDur: " << vDuration << " aDur: " << aDuration << std::endl;
    
}

int main(int argc, char* argv[])
{
    try{
        Test::Suite ts;
        ts.add(std::auto_ptr<Test::Suite>(new DemuxerSuite()));

        Test::TextOutput output(Test::TextOutput::Verbose);
        ts.run(output, true);
    } catch (int e) {
        std::cout << "Unexpected exception encountered: " << e << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
} 
