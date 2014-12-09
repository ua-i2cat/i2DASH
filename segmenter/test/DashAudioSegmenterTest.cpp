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
#include <cpptest.h>

#include "DashAudioSegmenter.hh"

using namespace std;

class dashAudioSegmenterTestSuite : public Test::Suite {
protected:
    void tear_down();
    DashAudioSegmenter* aSeg;
};

class constructorTestSuite : public dashAudioSegmenterTestSuite {
public:
    constructorTestSuite() {
        TEST_ADD(constructorTestSuite::constructorTest);
    }
    
private:
    void constructorTest();
};

class initTestSuite : public dashAudioSegmenterTestSuite {
public:
    initTestSuite() {
        TEST_ADD(initTestSuite::init);
    }
    
private:
    void setup();
    void init();
};

class generateInitTestSuite : public dashAudioSegmenterTestSuite {
public:
    generateInitTestSuite() {
        TEST_ADD(generateInitTestSuite::generateInit);
    }
    
private:
    void setup();
    void generateInit();
    void tear_down();

    int inputDataSize;
    int outputDataSize;
    unsigned char* inputData;
    unsigned char* outputData;
};

void dashAudioSegmenterTestSuite::tear_down()
{
    delete aSeg;
}


void constructorTestSuite::constructorTest()
{
    aSeg = new DashAudioSegmenter();
    TEST_ASSERT(aSeg != NULL);
}

void initTestSuite::setup()
{
    aSeg = new DashAudioSegmenter();
}

void initTestSuite::init()
{
    if (aSeg == NULL) {
        TEST_FAIL("Segmenter instance is null. Check constructor test\n");
        return;
    }

    TEST_ASSERT_MSG(aSeg->init(), "AudioSegmenter init failed");
}

void generateInitTestSuite::setup()
{
    inputDataSize = 0;
    outputDataSize = 0;
    inputData = NULL;
    outputData = NULL;
    
    aSeg = new DashAudioSegmenter();
    if (aSeg == NULL) {
        TEST_FAIL("Segmenter instance is null. Check constructor test\n");
        return;
    }

    if (!aSeg->init()) {
        TEST_FAIL("Segmenter init failed. Check init test\n");
        return;
    }

    ifstream inputDataFile("testData/DashAudioSegmenterTest_input_data", ios::in|ios::binary|ios::ate);
    ifstream outputDataFile("testData/DashAudioSegmenterTest_init_model.m4a", ios::in|ios::binary|ios::ate);
    
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
    int diff = 0;
    size_t initBufferLen = 0;
    size_t bufferMaxLen = 1024*1024; //1MB
    unsigned char* initBuffer = new unsigned char[bufferMaxLen];

    initBufferLen = aSeg->generateInit(inputData, inputDataSize, initBuffer);

    if (initBufferLen != outputDataSize) {
        TEST_FAIL("Init buffer length invalid");
    }

    diff = memcmp(initBuffer, outputData, initBufferLen);

    TEST_ASSERT_MSG(diff == 0, "Init does not coincide with init file model");

    delete initBuffer;
}

void generateInitTestSuite::tear_down()
{
    delete inputData;
    delete outputData;
    delete aSeg;
}

int main(int argc, char* argv[])
{
    try{
        Test::Suite ts;
        ts.add(auto_ptr<Test::Suite>(new constructorTestSuite()));
        ts.add(auto_ptr<Test::Suite>(new initTestSuite()));
        ts.add(auto_ptr<Test::Suite>(new generateInitTestSuite()));

        Test::TextOutput output(Test::TextOutput::Verbose);
        ts.run(output, true);
    } catch (int e) {
        cout << "Unexpected exception encountered: " << e << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
} 