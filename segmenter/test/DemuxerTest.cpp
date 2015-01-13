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
#include <algorithm>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/XmlOutputter.h>

#include "Demuxer.hh"
#include "Frame.hh"

#define BIG_START_TIME 4294967296 // 2^32

class DemuxerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DemuxerTest);
    CPPUNIT_TEST(openCloseInput);
    CPPUNIT_TEST(dumpFormat);
    CPPUNIT_TEST(findStreams);
    CPPUNIT_TEST(readFrame);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void openCloseInput();
    void dumpFormat();
    void findStreams();
    void readFrame();

protected:
    uint64_t startTime;
    Demuxer* demux = NULL;
    std::string filePath = "testData/DemuxerTest_input_data.mp4";
    std::stringstream voidstream;
};

class BigDemuxerTest : public DemuxerTest
{
    CPPUNIT_TEST_SUITE(BigDemuxerTest);
    CPPUNIT_TEST(openCloseInput);
    CPPUNIT_TEST(dumpFormat);
    CPPUNIT_TEST(findStreams);
    CPPUNIT_TEST(readFrame);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

};

void DemuxerTest::setUp()
{
    demux = new Demuxer();
    startTime = 0;
    std::cerr.rdbuf(voidstream.rdbuf());
}

void DemuxerTest::tearDown()
{
    delete demux;
}

void BigDemuxerTest::setUp()
{
    demux = new Demuxer(BIG_START_TIME, BIG_START_TIME);
    startTime = BIG_START_TIME;
    std::cerr.rdbuf(voidstream.rdbuf());
}

void BigDemuxerTest::tearDown()
{
    delete demux;
}

void DemuxerTest::openCloseInput()
{
    CPPUNIT_ASSERT(demux != NULL);
    CPPUNIT_ASSERT(!demux->hasVideo());
    CPPUNIT_ASSERT(!demux->hasAudio());
    CPPUNIT_ASSERT(demux->openInput(filePath));

    CPPUNIT_ASSERT(!demux->openInput(filePath));
    CPPUNIT_ASSERT(!demux->hasVideo());
    CPPUNIT_ASSERT(!demux->hasAudio());
    demux->closeInput();
    CPPUNIT_ASSERT(!demux->isInputOpen());
}

void DemuxerTest::dumpFormat()
{
    CPPUNIT_ASSERT(demux != NULL);
    CPPUNIT_ASSERT(demux->openInput(filePath));

    demux->dumpFormat();
    demux->closeInput();
}

void DemuxerTest::findStreams()
{
    CPPUNIT_ASSERT(demux != NULL);
    CPPUNIT_ASSERT(demux->openInput(filePath));
    CPPUNIT_ASSERT(demux->findStreams());
    if (demux->hasVideo()){
        CPPUNIT_ASSERT(demux->getVideoBitRate() > 0);
        CPPUNIT_ASSERT(demux->getFPS() > 0);
        CPPUNIT_ASSERT(demux->getWidth() > 0);
        CPPUNIT_ASSERT(demux->getHeight() > 0);
        CPPUNIT_ASSERT(demux->getVideoDuration() > 0);
    }
    if (demux->hasAudio()){
        CPPUNIT_ASSERT(demux->getAudioBitRate() > 0);
        CPPUNIT_ASSERT(demux->getAudioSampleRate() > 0);
        CPPUNIT_ASSERT(demux->getAudioBitsPerSample() > 0);
        CPPUNIT_ASSERT(demux->getAudioChannels() > 0);
        CPPUNIT_ASSERT(demux->getAudioDuration() > 0);
    }
    demux->closeInput();
}

void DemuxerTest::readFrame()
{
    AVCCFrame* vFrame = NULL;
    AACFrame* aFrame = NULL;
    Frame* frame;
    int gotFrame;
    bool videoFrame = false;
    bool audioFrame = false;
    size_t aDuration;
    size_t vDuration;
    
    CPPUNIT_ASSERT(demux != NULL);
    CPPUNIT_ASSERT(demux->openInput(filePath));
    CPPUNIT_ASSERT(demux->findStreams());
    aDuration = demux->getAudioDuration();
    CPPUNIT_ASSERT(aDuration > 0);
    vDuration = demux->getVideoDuration();
    CPPUNIT_ASSERT(vDuration > 0);
    
    do {
        frame = demux->readFrame(gotFrame);
        if (dynamic_cast<AVCCFrame*>(frame)){
            vFrame = dynamic_cast<AVCCFrame*>(frame);
            videoFrame = true;
            CPPUNIT_ASSERT(vFrame->getDataBuffer() != NULL);
            CPPUNIT_ASSERT(vFrame->getDataLength() > 0);
            CPPUNIT_ASSERT(vFrame->getDuration() > 0);
        } else if (dynamic_cast<AACFrame*>(frame)){
            aFrame = dynamic_cast<AACFrame*>(frame);
            audioFrame = true;
            CPPUNIT_ASSERT(aFrame->getDataBuffer() != NULL);
            CPPUNIT_ASSERT(aFrame->getDataLength() > 0);
            CPPUNIT_ASSERT(aFrame->getDuration() > 0);
        }
    } while(gotFrame >= 0);
    
    if (demux->hasVideo()){
        CPPUNIT_ASSERT(videoFrame == true);
        CPPUNIT_ASSERT(vFrame != NULL);
        CPPUNIT_ASSERT(vFrame->getPresentationTime() > startTime);
    } 
    
    if (demux->hasAudio()){
        CPPUNIT_ASSERT(audioFrame == true);
        CPPUNIT_ASSERT(aFrame != NULL);
        CPPUNIT_ASSERT(aFrame->getPresentationTime() > startTime);
    }
    
    std::cout << " vDur: " << vDuration << " aDur: " << aDuration << std::endl;
    
    demux->closeInput();
}

CPPUNIT_TEST_SUITE_REGISTRATION( DemuxerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( BigDemuxerTest );

int main(int argc, char* argv[])
{
    std::ofstream xmlout("DemuxerTestResult.xml");
    CPPUNIT_NS::TextTestRunner runner;
    CPPUNIT_NS::XmlOutputter *outputter = new CPPUNIT_NS::XmlOutputter(&runner.result(), xmlout);

    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( "", false );
    outputter->write();

    return runner.result().wasSuccessful() ? 0 : 1;
} 
