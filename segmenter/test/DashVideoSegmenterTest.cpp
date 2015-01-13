/*
 *  DashVideoSegmenterTest.cpp - DashVideoSegmenter class test
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

#include "DashVideoSegmenter.hh"

#define TEST_SEGMENT_DURATION 127488
#define TEST_VIDEO_TIME_BASE 12800
#define TEST_VIDEO_SAMPLE_DURATION 512
#define TEST_VIDEO_WIDTH 1280
#define TEST_VIDEO_HEIGHT 720
#define TEST_VIDEO_FPS 25
#define TEST_FRAME_PTS 1024
#define TEST_FRAME_DTS 0
#define TEST_FRAME_DURATION 512
#define TEST_FRAME_IS_INTRA false

using namespace std;

class dashVideoSegmenterTestSuite : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(dashVideoSegmenterTestSuite);
    CPPUNIT_TEST(init);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void init();

    DashVideoSegmenter* vSeg;
};

class generateInitTestSuite : public dashVideoSegmenterTestSuite
{
    CPPUNIT_TEST_SUITE(generateInitTestSuite);
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

class generateSegmentTestSuite : public dashVideoSegmenterTestSuite
{
    CPPUNIT_TEST_SUITE(generateSegmentTestSuite);
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
    AVCCFrame* frame;
};


void dashVideoSegmenterTestSuite::setUp()
{
    vSeg = new DashVideoSegmenter();
    if (vSeg == NULL) {
        CPPUNIT_FAIL("Cannot instantiate DashVideoSegmenter. Out of memory!?\n");
    }
}

void dashVideoSegmenterTestSuite::tearDown()
{
    delete vSeg;
}

void dashVideoSegmenterTestSuite::init()
{
    CPPUNIT_ASSERT_MESSAGE("VideoSegmenter init failed",
            vSeg->init(TEST_SEGMENT_DURATION, TEST_VIDEO_TIME_BASE, TEST_VIDEO_SAMPLE_DURATION, TEST_VIDEO_WIDTH, TEST_VIDEO_HEIGHT, TEST_VIDEO_FPS));
}

void generateInitTestSuite::setUp()
{
    inputDataSize = 0;
    outputDataSize = 0;
    inputData = NULL;
    outputData = NULL;
    
    vSeg = new DashVideoSegmenter();
    if (vSeg == NULL) {
        CPPUNIT_FAIL("Cannot instantiate DashAudioSegmenter. Out of memory!?\n");
        return;
    }

    if (!vSeg->init(TEST_SEGMENT_DURATION, TEST_VIDEO_TIME_BASE, TEST_VIDEO_SAMPLE_DURATION, TEST_VIDEO_WIDTH, TEST_VIDEO_HEIGHT, TEST_VIDEO_FPS)) {
        CPPUNIT_FAIL("Segmenter init failed. Check init test\n");
        return;
    }

    ifstream inputDataFile("testData/DashVideoSegmenterTest_input_data", ios::in|ios::binary|ios::ate);
    ifstream outputDataFile("testData/DashVideoSegmenterTest_init_model.m4v", ios::in|ios::binary|ios::ate);
    
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

void generateInitTestSuite::generateInit()
{
    std::string dummyPath("");
    int dummySeqNumber = 0;
    DashSegment* initSegment = new DashSegment(dummyPath, vSeg->getMaxSegmentLength(), dummySeqNumber);

    int diff = 0;

    vSeg->generateInit(inputData, inputDataSize, initSegment);

    if (initSegment->getDataLength() != outputDataSize) {
        CPPUNIT_FAIL("Init buffer length invalid");
    }

    diff = memcmp(initSegment->getDataBuffer(), outputData, initSegment->getDataLength());

    CPPUNIT_ASSERT_MESSAGE("Init does not coincide with init file model", diff == 0);

    delete initSegment;
}

void generateInitTestSuite::tearDown()
{
    delete inputData;
    delete outputData;
    delete vSeg;
}

void generateSegmentTestSuite::setUp()
{
    vSeg = new DashVideoSegmenter();
    if (vSeg == NULL) {
        CPPUNIT_FAIL("Cannot instantiate DashAudioSegmenter. Out of memory!?\n");
        return;
    }

    if (!vSeg->init(TEST_SEGMENT_DURATION, TEST_VIDEO_TIME_BASE, TEST_VIDEO_SAMPLE_DURATION, TEST_VIDEO_WIDTH, TEST_VIDEO_HEIGHT, TEST_VIDEO_FPS)) {
        CPPUNIT_FAIL("Segmenter init failed. Check init test\n");
        return;
    }

    std::string dummyPath("");
    int dummySeqNumber = 0;
    int maxData = vSeg->getMaxSegmentLength();
    unsigned char* dummyBuffer = new unsigned char[maxData];
    
    frame = new AVCCFrame();
    segment = new DashSegment(dummyPath, maxData, dummySeqNumber);

    frame->setDataBuffer(dummyBuffer, maxData);
    frame->setPresentationTime(TEST_FRAME_PTS);
    frame->setDecodeTime(TEST_FRAME_DTS);
    frame->setDuration(TEST_FRAME_DURATION);
    frame->setIntra(TEST_FRAME_IS_INTRA);
}

void generateSegmentTestSuite::addToSegment()
{
    CPPUNIT_ASSERT_MESSAGE("Frame duration larger than segment duration", TEST_SEGMENT_DURATION > TEST_FRAME_DURATION);
    CPPUNIT_ASSERT_MESSAGE("AddToSegment detected segment end too early", !vSeg->addToSegment(frame, segment));
}

void generateSegmentTestSuite::finishSegment()
{
    CPPUNIT_ASSERT_MESSAGE("FinishSegment returned true with no segment data prepared", !vSeg->finishSegment(segment));
    CPPUNIT_ASSERT_MESSAGE("AddToSegment detected segment end too early", !vSeg->addToSegment(frame, segment));
    CPPUNIT_ASSERT_MESSAGE("FinishSegment failed", vSeg->finishSegment(segment));
}

void generateSegmentTestSuite::tearDown()
{
    delete vSeg;
    delete frame;
    delete segment;
}

CPPUNIT_TEST_SUITE_REGISTRATION( dashVideoSegmenterTestSuite );
CPPUNIT_TEST_SUITE_REGISTRATION( generateInitTestSuite );
CPPUNIT_TEST_SUITE_REGISTRATION( generateSegmentTestSuite );

int main(int argc, char* argv[])
{
    std::ofstream xmlout("DashVideoSegmenterTestResult.xml");
    CPPUNIT_NS::TextTestRunner runner;
    CPPUNIT_NS::XmlOutputter *outputter = new CPPUNIT_NS::XmlOutputter(&runner.result(), xmlout);

    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( "", false );
    outputter->write();

    return runner.result().wasSuccessful() ? 0 : 1;
} 
