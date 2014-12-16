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
#include <cpptest.h>

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

class dashVideoSegmenterTestSuite : public Test::Suite {
protected:
    void tear_down();
    DashVideoSegmenter* vSeg;
};

class constructorTestSuite : public dashVideoSegmenterTestSuite {
public:
    constructorTestSuite() {
        TEST_ADD(constructorTestSuite::constructorTest);
    }
    
private:
    void constructorTest();
};

class initTestSuite : public dashVideoSegmenterTestSuite {
public:
    initTestSuite() {
        TEST_ADD(initTestSuite::init);
    }
    
private:
    void setup();
    void init();
};

class generateInitTestSuite : public dashVideoSegmenterTestSuite {
public:
    generateInitTestSuite() {
        TEST_ADD(generateInitTestSuite::generateInit);
    }
    
private:
    void setup();
    void generateInit();
    void tear_down();

    size_t inputDataSize;
    size_t outputDataSize;
    unsigned char* inputData;
    unsigned char* outputData;
};

class generateSegmentTestSuite : public dashVideoSegmenterTestSuite {
public:
    generateSegmentTestSuite() {
        TEST_ADD(generateSegmentTestSuite::addToSegment);
        TEST_ADD(generateSegmentTestSuite::finishSegment);
    }
    
private:
    void setup();
    void addToSegment();
    void finishSegment();
    void tear_down();

    DashSegment* segment;
    AVCCFrame* frame;

};


void dashVideoSegmenterTestSuite::tear_down()
{
    delete vSeg;
}


void constructorTestSuite::constructorTest()
{
    vSeg = new DashVideoSegmenter();
    TEST_ASSERT(vSeg != NULL);
}

void initTestSuite::setup()
{
    vSeg = new DashVideoSegmenter();
}

void initTestSuite::init()
{
    if (vSeg == NULL) {
        TEST_FAIL("Segmenter instance is null. Check constructor test\n");
        return;
    }

    TEST_ASSERT_MSG(vSeg->init(TEST_SEGMENT_DURATION, TEST_VIDEO_TIME_BASE, TEST_VIDEO_SAMPLE_DURATION, TEST_VIDEO_WIDTH, TEST_VIDEO_HEIGHT, TEST_VIDEO_FPS), 
                     "VideoSegmenter init failed");
}

void generateInitTestSuite::setup()
{
    inputDataSize = 0;
    outputDataSize = 0;
    inputData = NULL;
    outputData = NULL;
    
    vSeg = new DashVideoSegmenter();
    if (vSeg == NULL) {
        TEST_FAIL("Segmenter instance is null. Check constructor test\n");
        return;
    }

    if (!vSeg->init(TEST_SEGMENT_DURATION, TEST_VIDEO_TIME_BASE, TEST_VIDEO_SAMPLE_DURATION, TEST_VIDEO_WIDTH, TEST_VIDEO_HEIGHT, TEST_VIDEO_FPS)) {
        TEST_FAIL("Segmenter init failed. Check init test\n");
        return;
    }

    ifstream inputDataFile("testData/DashVideoSegmenterTest_input_data", ios::in|ios::binary|ios::ate);
    ifstream outputDataFile("testData/DashVideoSegmenterTest_init_model.m4v", ios::in|ios::binary|ios::ate);
    
    if (!inputDataFile.is_open() || !outputDataFile.is_open()) {
        TEST_FAIL("Error opening test files. Check paths\n");
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
        TEST_FAIL("Error filling buffers from files");
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
        TEST_FAIL("Init buffer length invalid");
    }

    diff = memcmp(initSegment->getDataBuffer(), outputData, initSegment->getDataLength());

    TEST_ASSERT_MSG(diff == 0, "Init does not coincide with init file model");

    delete initSegment;
}

void generateInitTestSuite::tear_down()
{
    delete inputData;
    delete outputData;
    delete vSeg;
}

void generateSegmentTestSuite::setup()
{
    vSeg = new DashVideoSegmenter();
    if (vSeg == NULL) {
        TEST_FAIL("Segmenter instance is null. Check constructor test\n");
        return;
    }

    if (!vSeg->init(TEST_SEGMENT_DURATION, TEST_VIDEO_TIME_BASE, TEST_VIDEO_SAMPLE_DURATION, TEST_VIDEO_WIDTH, TEST_VIDEO_HEIGHT, TEST_VIDEO_FPS)) {
        TEST_FAIL("Segmenter init failed. Check init test\n");
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
    TEST_ASSERT_MSG(TEST_SEGMENT_DURATION > TEST_FRAME_DURATION, "Frame duration larger than segment duration");
    TEST_ASSERT_MSG(!vSeg->addToSegment(frame, segment), "AddToSegment detected segment end too early");
}

void generateSegmentTestSuite::finishSegment()
{
    TEST_ASSERT_MSG(!vSeg->finishSegment(segment), "FinishSegment returned true with no segment data prepared");
    TEST_ASSERT_MSG(!vSeg->addToSegment(frame, segment), "AddToSegment detected segment end too early");
    TEST_ASSERT_MSG(vSeg->finishSegment(segment), "FinishSegment failed");
}

void generateSegmentTestSuite::tear_down()
{
    delete vSeg;
    delete frame;
    delete segment;
}

int main(int argc, char* argv[])
{
    try{
        Test::Suite ts;
        ts.add(auto_ptr<Test::Suite>(new constructorTestSuite()));
        ts.add(auto_ptr<Test::Suite>(new initTestSuite()));
        ts.add(auto_ptr<Test::Suite>(new generateInitTestSuite()));
        ts.add(auto_ptr<Test::Suite>(new generateSegmentTestSuite()));

        Test::TextOutput output(Test::TextOutput::Verbose);
        ts.run(output, true);
    } catch (int e) {
        cout << "Unexpected exception encountered: " << e << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
} 