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
#include <cpptest.h>

#include "DashVideoSegmenter.hh"

using namespace std;

class constructorTestSuite : public Test::Suite {
public:
    constructorTestSuite() {
        TEST_ADD(constructorTestSuite::constructorTest);
    }
    
private:
    DashVideoSegmenter* vSeg;
    void constructorTest();
};

class initTestSuite : public Test::Suite {
public:
    initTestSuite() {
        TEST_ADD(initTestSuite::init);
    }
    
private:
    DashVideoSegmenter* vSeg;
    int size;
    char* 
    void setup();
    void init();
};

class generateInitTestSuite : public Test::Suite {
public:
    generateInitTestSuite() {
        TEST_ADD(generateInitTestSuite::generateInit);
    }
    
private:
    void setup();
    void generateInit();
};

/*class destructorTestSuite : public Test::Suite {
public:
    destructorTestSuite() {
        TEST_ADD(destructorTestSuite::constructorTest);
    }
    
private:
    void destructorTest();
};*/


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

    TEST_ASSERT_MSG(vSeg->init(), "VideoSegmenter init failed");
}

void generateInitTestSuite::setup()
{
    if (vSeg == NULL) {
        TEST_FAIL("Segmenter instance is null. Check constructor test\n");
        return;
    }

    if (!vSeg->init()) {
        TEST_FAIL("Segmenter init failed. Check init test\n");
        return;
    }

    ifstream file("path/to/extradata", ios::in|ios::binary|ios::ate);
    
    if (!file.is_open()) {
        TEST_FAIL("Error opening test file. Check path\n");
        return;
    }

    size = file.tellg();
    memblock = new char[size];
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();
}

void generateInitTestSuite::generateInit()
{
    TEST_ASSERT_MSG(vSeg->init(), "VideoSegmenter init failed");
}

int main(int argc, char* argv[])
{
    try{
        Test::Suite ts;
        ts.add(auto_ptr<Test::Suite>(new constructorTestSuite()));
        ts.add(auto_ptr<Test::Suite>(new initTestSuite()));

        Test::TextOutput output(Test::TextOutput::Verbose);
        ts.run(output, true);
    } catch (int e) {
        cout << "Unexpected exception encountered: " << e << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
} 