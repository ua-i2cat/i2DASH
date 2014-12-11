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

bool isIntra(unsigned char* data);
size_t getNalSizeBytes(unsigned char* metadata);   


Demuxer::Demuxer(uint64_t vTime, uint64_t aTime): fmtCtx(NULL), videoStreamIdx(-1), 
                    audioStreamIdx(-1), framesCounter(0), isOpen(false), 
                    videoBitRate(0), audioBitRate(0), fps(0.0), width(0),
                    height(0), channels(0), sampleRate(0), bitsPerSample(0),
                    vStartTime(aTime), aStartTime(aTime), 
                    videoFrame(new AVCCFrame()), audioFrame(new AACFrame())
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

    if (videoStreamIdx < 0 || videoStreamIdx == audioStreamIdx) {
        videoStreamIdx = -1;
        return false;
    }
        
    videoBitRate = fmtCtx->streams[videoStreamIdx]->codec->bit_rate;
    width = fmtCtx->streams[videoStreamIdx]->codec->width;
    height = fmtCtx->streams[videoStreamIdx]->codec->height;
    fps = (float) fmtCtx->streams[videoStreamIdx]->avg_frame_rate.num / (float) fmtCtx->streams[videoStreamIdx]->avg_frame_rate.den;

    if (fps <= 0 || videoBitRate <= 0 || width <= 0 || height <= 0) {
        return false;
    }

    if (!validVideoCodec()) {
        return false;
    }

    if (fmtCtx->streams[videoStreamIdx]->codec->extradata_size <= 0) {
        return false;
    }

    nalSizeBytes = getNalSizeBytes(fmtCtx->streams[videoStreamIdx]->codec->extradata)   
    
    return true;
}

bool Demuxer::findAudioStream()
{
    if (!isOpen){
        return false;
    }
    
    audioStreamIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioStreamIdx >= 0 && videoStreamIdx != audioStreamIdx){
        
        audioBitRate = fmtCtx->streams[audioStreamIdx]->codec->bit_rate;
        channels = fmtCtx->streams[audioStreamIdx]->codec->channels;
        sampleRate = fmtCtx->streams[audioStreamIdx]->codec->sample_rate;
        bitsPerSample = av_get_bytes_per_sample(fmtCtx->streams[audioStreamIdx]->codec->sample_fmt) * 8; //Bytes to bits;

        if (validAudioCodec() && channels > 0 && sampleRate > 0 && sampleRate > 0 && audioBitRate > 0) {
            return true;
        }
    }
    
    audioStreamIdx = -1;
    
    return false;
}

bool Demuxer::validVideoCodec()
{
    if (!isOpen || videoStreamIdx < 0){
        return false;
    }
    
    if (fmtCtx->streams[videoStreamIdx]->codec->codec_id != CODEC_ID_H264){
        return false;
    }
    
    return true;
}

bool Demuxer::validAudioCodec()
{
    if (!isOpen || audioStreamIdx < 0){
        return false;
    }
    
    if (fmtCtx->streams[audioStreamIdx]->codec->codec_id != CODEC_ID_AAC){
        cerr << "Invalid codec. It must be AAC" << endl;
        return false;
    }
       
    return true;
}


bool Demuxer::hasVideo()
{
    if (!isOpen || fmtCtx == NULL){
        return false;
    }
    
    return videoStreamIdx >= 0;
}

bool Demuxer::hasAudio()
{
    if (!isOpen || fmtCtx == NULL){
        return false;
    }
    
    return audioStreamIdx >= 0;
}

unsigned char* Demuxer::getVideoExtraData()
{
    unsigned char* data = NULL;

    if (!isOpen || fmtCtx == NULL || videoStreamIdx < 0) {
        return data;
    }

    if (!fmtCtx->streams[videoStreamIdx]->codec) {
        return data;
    }

    if (!fmtCtx->streams[videoStreamIdx]->codec->extradata) {
        return data;
    }

    data = fmtCtx->streams[videoStreamIdx]->codec->extradata;

    return data;
}

size_t Demuxer::getVideoExtraDataLength()
{
    size_t size = 0;

    if (!isOpen || fmtCtx == NULL || videoStreamIdx < 0) {
        return size;
    }

    if (!fmtCtx->streams[videoStreamIdx]->codec) {
        return size;
    }

    if (!fmtCtx->streams[videoStreamIdx]->codec->extradata) {
        return size;
    }

    size = fmtCtx->streams[videoStreamIdx]->codec->extradata_size;

    return size;
}

unsigned char* Demuxer::getAudioExtraData()
{
     unsigned char* data = NULL;

    if (!isOpen || fmtCtx == NULL || audioStreamIdx < 0) {
        return data;
    }

    if (!fmtCtx->streams[audioStreamIdx]->codec) {
        return data;
    }

    if (!fmtCtx->streams[audioStreamIdx]->codec->extradata) {
        return data;
    }

    data = fmtCtx->streams[audioStreamIdx]->codec->extradata;

    return data;
}

size_t Demuxer::getAudioExtraDataLength()
{
    size_t size = 0;

    if (!isOpen || fmtCtx == NULL || audioStreamIdx < 0) {
        return size;
    }

    if (!fmtCtx->streams[audioStreamIdx]->codec) {
        return size;
    }

    if (!fmtCtx->streams[audioStreamIdx]->codec->extradata) {
        return size;
    }

    size = fmtCtx->streams[audioStreamIdx]->codec->extradata_size;

    return size;
}

Frame* const Demuxer::readFrame(int &gotFrame)
{
    uint64_t time;
    
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

            videoFrame->setIntra(isIntra(pkt.data));
            
            time = (uint64_t) ((double) pkt.pts * (double) fmtCtx->streams[videoStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[videoStreamIdx]->time_base.den * 1000.0) + vStartTime;
            
            videoFrame->setPresentationTime(std::chrono::milliseconds(time));

            return videoFrame;
        }

        if (pkt.stream_index == audioStreamIdx) {
            audioFrame->setDataBuffer(pkt.data, pkt.size);
            
            time = (uint64_t) ((double) pkt.pts * (double) fmtCtx->streams[audioStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[audioStreamIdx]->time_base.den * 1000.0) + aStartTime;
                
            audioFrame->setPresentationTime(std::chrono::milliseconds(time));

            return audioFrame;
        } 
    }
    
    return NULL;
}

size_t getNalSizeBytes(unsigned char* metadata)
{
    
}


bool isIntra(unsigned char* data)
{
    
}


