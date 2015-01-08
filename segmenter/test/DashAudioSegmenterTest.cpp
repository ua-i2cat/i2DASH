/*
 *  DashAudioSegmenterTest.cpp - DashAudioSegmenter class test
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
 *  Authors:  Marc Palau <marc.palau@i2cat.net>
 *            
 *            
 */

#include <string>
#include <iostream>
#include <fstream>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/XmlOutputter.h>

#include "DashAudioSegmenter.hh"

#define TEST_SEGMENT_DURATION 477184
#define TEST_AUDIO_TIME_BASE 48000
#define TEST_AUDIO_SAMPLE_DURATION 1024
#define TEST_AUDIO_CHANNELS 2
#define TEST_AUDIO_SAMPLE_RATE 48000
#define TEST_AUDIO_BITS_PER_SAMPLE 32
#define TEST_FRAME_PTS 1024
#define TEST_FRAME_DTS 0
#define TEST_FRAME_DURATION 512

using namespace std;

class dashAudioSegmenterTestSuite : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(dashAudioSegmenterTestSuite);
	CPPUNIT_TEST(init);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
    void tearDown();
    
protected:
    void constructor();
    void init();
    
    DashAudioSegmenter* aSeg;
};

class generateInitTestSuite : public dashAudioSegmenterTestSuite
{
	CPPUNIT_TEST_SUB_SUITE(generateInitTestSuite, dashAudioSegmenterTestSuite);
	CPPUNIT_TEST(generateInit);
	CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void generateInit();

private:
    size_t inputDataSize;
    size_t outputDataSize;
    unsigned char* inputData;
    unsigned char* outputData;
};

class generateSegmentTestSuite : public dashAudioSegmenterTestSuite
{
	CPPUNIT_TEST_SUB_SUITE(generateSegmentTestSuite, dashAudioSegmenterTestSuite);
	CPPUNIT_TEST(addToSegment);
	CPPUNIT_TEST(finishSegment);
	CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void addToSegment();
    void finishSegment();

private:
    DashSegment* segment;
    AACFrame* frame;
};

void dashAudioSegmenterTestSuite::setUp()
{
    aSeg = new DashAudioSegmenter();
}

void dashAudioSegmenterTestSuite::tearDown()
{
    delete aSeg;
}

void dashAudioSegmenterTestSuite::init()
{
    CPPUNIT_ASSERT_MESSAGE("AudioSegmenter init failed", aSeg->init(TEST_SEGMENT_DURATION, TEST_AUDIO_TIME_BASE, TEST_AUDIO_SAMPLE_DURATION,
                               TEST_AUDIO_CHANNELS, TEST_AUDIO_SAMPLE_RATE, TEST_AUDIO_BITS_PER_SAMPLE));
}

void generateInitTestSuite::setUp()
{
    inputDataSize = 0;
    outputDataSize = 0;
    inputData = NULL;
    outputData = NULL;
    
    aSeg = new DashAudioSegmenter();
    if (aSeg == NULL) {
        CPPUNIT_FAIL("Segmenter instance is null. Check constructor test\n");
        return;
    }

    if (!aSeg->init(TEST_SEGMENT_DURATION, TEST_AUDIO_TIME_BASE, TEST_AUDIO_SAMPLE_DURATION, 
                               TEST_AUDIO_CHANNELS, TEST_AUDIO_SAMPLE_RATE, TEST_AUDIO_BITS_PER_SAMPLE)) {
        CPPUNIT_FAIL("Segmenter init failed. Check init test\n");
        return;
    }

    ifstream inputDataFile("testData/DashAudioSegmenterTest_input_data", ios::in|ios::binary|ios::ate);
    ifstream outputDataFile("testData/DashAudioSegmenterTest_init_model.m4a", ios::in|ios::binary|ios::ate);
    
    if (!inputDataFile.is_open() || !outputDataFile.is_open()) {
        CPPUNIT_FAIL("Error opening test files. Check paths\n");
        return;
    }

    inputDataSize = inputDataFile.tellg();
    inputData = new unsigned char[inputDataSize];
    inputDataFile.seekg (0, ios::beg);
    inputDataFile.read((char*)inputData, inputDataSize);

    outputDataSize = outputDataFile.tellg();
    outputData = new unsigned char[outputDataSize];
    outputDataFile.seekg (0, ios::beg);
    outputDataFile.read((char *)outputData, outputDataSize);

    if (!inputData || !outputData) {
        CPPUNIT_FAIL("Error filling buffers from files");
        return;
    }

    inputDataFile.close();
    outputDataFile.close();
}

void generateInitTestSuite::tearDown()
{
    delete inputData;
    delete outputData;
}

void generateInitTestSuite::generateInit()
{
    std::string dummyPath("");
    int dummySeqNumber = 0;
    DashSegment* initSegment = new DashSegment(dummyPath, aSeg->getMaxSegmentLength(), dummySeqNumber);

    int diff = 0;

    aSeg->generateInit(inputData, inputDataSize, initSegment);

    if (initSegment->getDataLength() != outputDataSize) {
        CPPUNIT_FAIL("Init buffer length invalid");
    }

    diff = memcmp(initSegment->getDataBuffer(), outputData, initSegment->getDataLength());

    CPPUNIT_ASSERT_MESSAGE("Init does not coincide with init file model", diff == 0);

    delete initSegment;
}

void generateSegmentTestSuite::setUp()
{
   aSeg = new DashAudioSegmenter();
    if (aSeg == NULL) {
        CPPUNIT_FAIL("Segmenter instance is null. Check constructor test\n");
        return;
    }

    if (!aSeg->init(TEST_SEGMENT_DURATION, TEST_AUDIO_TIME_BASE, TEST_AUDIO_SAMPLE_DURATION, 
                               TEST_AUDIO_CHANNELS, TEST_AUDIO_SAMPLE_RATE, TEST_AUDIO_BITS_PER_SAMPLE)) {
        CPPUNIT_FAIL("Segmenter init failed. Check init test\n");
        return;
    }
    
    std::string dummyPath("");
    int dummySeqNumber = 0;
    int maxData = aSeg->getMaxSegmentLength();
    unsigned char* dummyBuffer = new unsigned char[maxData];
    
    CPPUNIT_ASSERT(dummyBuffer != NULL);

    frame = new AACFrame();
    segment = new DashSegment(dummyPath, maxData, dummySeqNumber);

    CPPUNIT_ASSERT(frame != NULL);
    CPPUNIT_ASSERT(segment != NULL);

    frame->setDataBuffer(dummyBuffer, maxData);
    frame->setPresentationTime(TEST_FRAME_PTS);
    frame->setDecodeTime(TEST_FRAME_DTS);
    frame->setDuration(TEST_FRAME_DURATION);
}

void generateSegmentTestSuite::tearDown()
{
    delete frame;
    delete segment;
}

void generateSegmentTestSuite::addToSegment()
{
    CPPUNIT_ASSERT_MESSAGE("Frame duration larger than segment duration", TEST_SEGMENT_DURATION > TEST_FRAME_DURATION);
    CPPUNIT_ASSERT_MESSAGE("AddToSegment detected segment end too early", !aSeg->addToSegment(frame, segment));
}

void generateSegmentTestSuite::finishSegment()
{
	CPPUNIT_ASSERT_MESSAGE("FinishSegment returned true with no segment data prepared", !aSeg->finishSegment(segment));
	CPPUNIT_ASSERT_MESSAGE("AddToSegment detected segment end too early", !aSeg->addToSegment(frame, segment));
	CPPUNIT_ASSERT_MESSAGE("FinishSegment failed", aSeg->finishSegment(segment));
}

CPPUNIT_TEST_SUITE_REGISTRATION( dashAudioSegmenterTestSuite );
CPPUNIT_TEST_SUITE_REGISTRATION( generateInitTestSuite );
CPPUNIT_TEST_SUITE_REGISTRATION( generateSegmentTestSuite );

int main(int argc, char* argv[])
{
	std::ofstream xmlout("DashAudioSegmenterTestResult.xml");
    CPPUNIT_NS::TextTestRunner runner;
    CPPUNIT_NS::XmlOutputter *outputter = new CPPUNIT_NS::XmlOutputter(&runner.result(), xmlout);

    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( "", false );
    outputter->write();

    return runner.result().wasSuccessful() ? 0 : 1;
} 
