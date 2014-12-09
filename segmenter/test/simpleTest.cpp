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

using namespace std;

int main(int argc, char* argv[])
{
    Demuxer* demux = NULL;
    DashVideoSegmenter* vSeg = NULL;
    DashAudioSegmenter* aSeg = NULL;
    size_t bufferMaxLen = 1024*1024; //1MB
    size_t vInitBufferLen = 0;
    size_t aInitBufferLen = 0;
    unsigned char* vInitBuffer = NULL;
    unsigned char* aInitBuffer = NULL;

    AVCCFrame* videoFrame;
    AACFrame* audioFrame;
    Frame* frame;
    int gotFrame;

    demux = new Demuxer();
    vSeg = new DashVideoSegmenter();
    aSeg = new DashAudioSegmenter();

    vInitBuffer = new unsigned char[bufferMaxLen];
    aInitBuffer = new unsigned char[bufferMaxLen];

    ofstream vInitFile("testData/DashVideoSegmenterTest_init_model.m4v", ofstream::binary);
    ofstream aInitFile("testData/DashAudioSegmenterTest_init_model.m4a", ofstream::binary);
    ofstream vHeaderFile("testData/DashVideoSegmenterTest_input_data", ofstream::binary);
    ofstream aHeaderFile("testData/DashAudioSegmenterTest_input_data", ofstream::binary);

    demux->openInput(argv[1]);
    demux->findStreams();
    demux->dumpFormat();

    if (!vSeg->init()) {
        cout << "Error initializing Video Segmenter" << endl;
        exit(1);
    }

    if (!aSeg->init()) {
        cout << "Error initializing Audio Segmenter" << endl;
        exit(1);
    }

    // while (gotFrame >= 0){
    while (vInitBufferLen == 0 || aInitBufferLen == 0) {

        frame = demux->readFrame(gotFrame);

        if ((videoFrame = dynamic_cast<AVCCFrame*>(frame)) != NULL) {
            vInitBufferLen = vSeg->generateInit(videoFrame->getHdrBuffer(), videoFrame->getHdrLength(), vInitBuffer);
            vInitFile.write((char*)vInitBuffer,vInitBufferLen);
            vHeaderFile.write((char*)videoFrame->getHdrBuffer(),videoFrame->getHdrLength());
        }

        if ((audioFrame = dynamic_cast<AACFrame*>(frame)) != NULL) {
            aSeg->setSampleRate(audioFrame->getSampleRate());
            aInitBufferLen = aSeg->generateInit(audioFrame->getHdrBuffer(), audioFrame->getHdrLength(), aInitBuffer);
            aInitFile.write((char*)aInitBuffer,aInitBufferLen);
            aHeaderFile.write((char*)audioFrame->getHdrBuffer(),audioFrame->getHdrLength());
        }
    }

    vInitFile.close();
    aInitFile.close();
    vHeaderFile.close();
    aHeaderFile.close();

    delete demux;
    delete vSeg;
    delete aSeg;
    
    return 0;
} 
