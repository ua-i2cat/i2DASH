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

#include "Demuxer.hh"
#include "Frame.hh"

using namespace std;

class DemuxerSuite : public Test::Suite
{
public:
    DemuxerSuite(int argc, char **argv)
    {
        //TODO: check the best way to pass parameters to the test suite
        argc_ = argc;
        argv_ = argv;
        TEST_ADD(DemuxerSuite::correctParams)
        TEST_ADD(DemuxerSuite::constructorTest)
        TEST_ADD(DemuxerSuite::openInput)
        TEST_ADD(DemuxerSuite::dumpFormat)
        TEST_ADD(DemuxerSuite::findStreams)
        TEST_ADD(DemuxerSuite::readFrame)
        TEST_ADD(DemuxerSuite::closeInput)
        TEST_ADD(DemuxerSuite::destructorTest)
    }
    
private:
    int argc_;
    char **argv_;
    Demuxer* demux = NULL;
    void constructorTest();
    void correctParams();
    void destructorTest();
    void openInput();
    void closeInput();
    void dumpFormat();
    void findStreams();
    void readFrame();
};

void DemuxerSuite::correctParams()
{
    TEST_ASSERT(argc_ >= 2);
}

void DemuxerSuite::constructorTest()
{
    demux = new Demuxer();
    TEST_ASSERT(demux != NULL);
}


void DemuxerSuite::destructorTest()
{
    TEST_ASSERT(demux != NULL);
    delete demux;
}

void DemuxerSuite::openInput()
{
    TEST_ASSERT(demux != NULL);
    TEST_ASSERT(demux->openInput(argv_[1]));
    //TODO: redirect cerr
    TEST_ASSERT(!demux->openInput(argv_[1]));
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
    TEST_ASSERT(!demux->openInput(argv_[1]));
    //TODO: assert format is correct
    demux->dumpFormat();
}

void DemuxerSuite::findStreams()
{
    TEST_ASSERT(demux != NULL);
    TEST_ASSERT(demux->findVideoStream());
    TEST_ASSERT(demux->findAudioStream());
}

void DemuxerSuite::readFrame()
{
    AVCCFrame* frame;
    int gotFrame;
    bool videoFrame = false;
    TEST_ASSERT(demux != NULL);
    TEST_ASSERT(!demux->openInput(argv_[1]));
    TEST_ASSERT(demux->findVideoStream());
    TEST_ASSERT(demux->findAudioStream());
    do {
        frame = dynamic_cast<AVCCFrame*>(demux->readFrame(gotFrame));
        cout << "read result: " << gotFrame << endl;
        if (frame == NULL){
            cout << "NULL frame" << endl;
        }
        if (gotFrame >= 0 && frame != NULL){
            cout << "This is a frame" << endl;
            videoFrame = true;
            TEST_ASSERT(frame->getWidth() > 0);
            TEST_ASSERT(frame->getHeight() > 0);
            TEST_ASSERT(frame->getFrameBuf() != NULL);
            TEST_ASSERT(frame->getLength() > 0);
            TEST_ASSERT(frame->getFrameHBuf() != NULL);
            TEST_ASSERT(frame->getHLength() > 0);
        }
    }while(gotFrame >= 0);
    //TEST_ASSERT(videoFrame == true);
}

int main(int argc, char* argv[])
{    
    try{
        Test::Suite ts;
        ts.add(auto_ptr<Test::Suite>(new DemuxerSuite(argc, argv)));

        Test::TextOutput output(Test::TextOutput::Verbose);
        ts.run(output, true);
    } catch (int e) {
        cout << "Unexpected exception encountered: " << e << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
} 
