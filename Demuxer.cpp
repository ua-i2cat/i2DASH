/*
 *  Demuxer.cpp - Demuxer class
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


#include "Demuxer.hh"

#include <iostream>
#include <sys/stat.h>

Demuxer::Demuxer(): fmtCtx(NULL), videoStream(NULL), 
                                audioStream(NULL), videoStreamIdx(-1), 
                                audioStreamIdx(-1), framesCounter(0), 
                                isOpen(false), videoFrame(new AVCCFrame()),
                                audioFrame(new AACFrame())
{
    av_register_all();
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
}

Demuxer::~Demuxer()
{
    if (isOpen || fmtCtx != NULL){
        avformat_close_input(&fmtCtx);
    }
    av_free_packet(&pkt);
    
    delete videoFrame;
    delete audioFrame;
}

bool Demuxer::openInput(string filename)
{
    string format;
    
    if (isOpen){
        cerr << "Input already open, close it first"<< endl;
        return false;
    }
    
    if (!sourceExists(filename) || avformat_open_input(&fmtCtx, filename.c_str(), NULL, NULL) < 0) {
        cerr << "Could not open source file " << filename << endl;
        fmtCtx = NULL;
        return false;
    }
    
    format = fmtCtx->iformat->name;
    if (!(format.find("mp4") != string::npos)) {
        avformat_close_input(&fmtCtx);
        fmtCtx = NULL;
        return false;
    }
    
    isOpen = true;
    return true;
}

void Demuxer::closeInput()
{
    if (!isOpen || fmtCtx == NULL){
        cerr << "Input already closed"<< endl;
        pkt.size = 0;    
        pkt.data = NULL;
        return;
    }
    
    avformat_close_input(&fmtCtx);
  
    fmtCtx = NULL;
    isOpen = false;
}

bool Demuxer::sourceExists(string filename)
{
    struct stat buffer;
    return (stat (filename.c_str(), &buffer) == 0);
}

void Demuxer::dumpFormat()
{
    if (isOpen){
        av_dump_format(fmtCtx, 0, NULL, 0);
    }
}

bool Demuxer::findStreams()
{
    if (!isOpen){
        return false;
    }
    
    if (videoStreamIdx != -1 || audioStreamIdx != -1){
        cerr << "Streams already found"<< endl;
        return true;
    }
    
    if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
        cerr << "No streams found!"<< endl; 
        return false;
    }
    
    if (findVideoStream() | findAudioStream()){
        return true;
    }
    
    return false;
}

bool Demuxer::findVideoStream()
{
    if (!isOpen){
        return false;
    }
    
    videoStreamIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoStreamIdx >= 0 && videoStreamIdx != audioStreamIdx){
        videoStream = fmtCtx->streams[videoStreamIdx];
        if (validVideoCodec()){
            return true;
        }   
    } 
    
    videoStreamIdx = -1;
    
    return false;
}

bool Demuxer::validVideoCodec()
{
    if (!isOpen || videoStream == NULL){
        return false;
    }
    
    if (videoStream->codec->codec_id != CODEC_ID_H264){
        return false;
    }
    
    return true;
}

bool Demuxer::validAudioCodec()
{
    if (!isOpen || audioStream == NULL){
        return false;
    }
    
    if (audioStream->codec->codec_id != CODEC_ID_AAC){
        cerr << "Invalid codec. It must be AAC" << endl;
        return false;
    }
       
    return true;
}

bool Demuxer::findAudioStream()
{
    if (!isOpen){
        return false;
    }
    
    audioStreamIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioStreamIdx >= 0 && videoStreamIdx != audioStreamIdx){
        audioStream = fmtCtx->streams[audioStreamIdx];
        if (validAudioCodec()){
            return true;
        }
    }
    
    audioStreamIdx = -1;
    
    return false;
}

Frame* Demuxer::readFrame(int &gotFrame)
{
    gotFrame = -1;
    
    if (!isOpen){
        cerr << "Input closed" << endl;
        return NULL;
    }
    
    if (videoStreamIdx == -1 && audioStreamIdx == -1){
        cerr << "No audio or video streams found" << endl;
        return NULL;
    }

    pkt.size = 0;
    gotFrame = av_read_frame(fmtCtx, &pkt);
    
    if (pkt.size <= 0){
        return NULL;
    }
    
    if (gotFrame >= 0) {
        videoFrame->clearFrame();
        audioFrame->clearFrame();

        if (pkt.stream_index == videoStreamIdx) {
            videoFrame->setDataBuffer(pkt.data, pkt.size);
            if (videoStream->codec->extradata_size > 0){
                videoFrame->setHdrBuffer(videoStream->codec->extradata, 
                                   videoStream->codec->extradata_size);
            }
            videoFrame->setVideoSize(videoStream->codec->width, videoStream->codec->height);
            //TODO: set timestamp, bitrate, others...
            return videoFrame;
        }

        if (pkt.stream_index == audioStreamIdx) {
            audioFrame->setDataBuffer(pkt.data, pkt.size);
            if (audioStream->codec->extradata_size > 0){
                audioFrame->setHdrBuffer(audioStream->codec->extradata, 
                                       audioStream->codec->extradata_size);
            }

            audioFrame->setSampleRate(audioStream->codec->sample_rate);

            //TODO: set other parameters
            return audioFrame;
        } 

        cout << "Unknown frame, size: " << pkt.size << " idx: " << pkt.stream_index << endl;
        
    }
    
    return NULL;
}

