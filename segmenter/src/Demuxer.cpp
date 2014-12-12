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

Demuxer::Demuxer(uint64_t vTime, uint64_t aTime): fmtCtx(NULL), videoStreamIdx(-1), 
                    audioStreamIdx(-1), framesCounter(0), isOpen(false), 
                    videoBitRate(0), audioBitRate(0), fps(0.0), width(0),
                    height(0), channels(0), sampleRate(0), bitsPerSample(0),
                    vStartTime(aTime), aStartTime(aTime), videoFrame(new AVCCFrame()), 
                    audioFrame(new AACFrame()), nalSizeBytes(0)
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

bool Demuxer::openInput(std::string filename)
{
    std::string format;
    
    if (isOpen){
        std::cerr << "Input already open, close it first"<< std::endl;
        return false;
    }
    
    if (!sourceExists(filename) || avformat_open_input(&fmtCtx, filename.c_str(), NULL, NULL) < 0) {
        std::cerr << "Could not open source file " << filename << std::endl;
        fmtCtx = NULL;
        return false;
    }
    
    format = fmtCtx->iformat->name;
    if (!(format.find("mp4") != std::string::npos)) {
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
        std::cerr << "Input already closed"<< std::endl;
        pkt.size = 0;    
        pkt.data = NULL;
        return;
    }
    
    avformat_close_input(&fmtCtx);
  
    fmtCtx = NULL;
    isOpen = false;
}

bool Demuxer::sourceExists(std::string filename)
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
        std::cerr << "Streams already found"<< std::endl;
        return true;
    }
    
    if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
        std::cerr << "No streams found!"<< std::endl; 
        return false;
    }
    
    if (fmtCtx->duration <= 0) {
        std::cerr << "Could not set stream duration"<< std::endl; 
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

    nalSizeBytes = getNalSizeBytes(fmtCtx->streams[videoStreamIdx]->codec->extradata);   
    
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
        std::cerr << "Invalid video codec. IT must be H264" << std::endl;
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
        std::cerr << "Invalid audio codec. It must be AAC" << std::endl;
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
    size_t pTime;
    size_t dTime;
    size_t duration;
    
    gotFrame = -1;
    
    if (!isOpen){
        std::cerr << "Input closed" << std::endl;
        return NULL;
    }
    
    if (videoStreamIdx == -1 && audioStreamIdx == -1){
        std::cerr << "No audio or video streams found" << std::endl;
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
            
            pTime = (size_t) ((double) pkt.pts * (double) fmtCtx->streams[videoStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[videoStreamIdx]->time_base.den * 1000.0) + vStartTime;

            dTime = (size_t) ((double) pkt.dts * (double) fmtCtx->streams[videoStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[videoStreamIdx]->time_base.den * 1000.0) + vStartTime;

            duration = (size_t) ((double) pkt.duration * (double) fmtCtx->streams[videoStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[videoStreamIdx]->time_base.den * 1000.0) + vStartTime;
            
            videoFrame->setPresentationTime(std::chrono::milliseconds(pTime));
            videoFrame->setDecodeTime(std::chrono::milliseconds(dTime));
            videoFrame->setDuration(std::chrono::milliseconds(duration));

            return videoFrame;
        }

        if (pkt.stream_index == audioStreamIdx) {
            audioFrame->setDataBuffer(pkt.data, pkt.size);
            
            pTime = (size_t) ((double) pkt.pts * (double) fmtCtx->streams[audioStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[audioStreamIdx]->time_base.den * 1000.0) + aStartTime;

            dTime = (size_t) ((double) pkt.dts * (double) fmtCtx->streams[audioStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[audioStreamIdx]->time_base.den * 1000.0) + aStartTime;

            duration = (size_t) ((double) pkt.duration * (double) fmtCtx->streams[audioStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[audioStreamIdx]->time_base.den * 1000.0) + vStartTime;

            audioFrame->setPresentationTime(std::chrono::milliseconds(pTime));
            audioFrame->setDecodeTime(std::chrono::milliseconds(dTime));
            audioFrame->setDuration(std::chrono::milliseconds(duration));

            return audioFrame;
        } 
    }
    
    return NULL;
}

std::chrono::milliseconds Demuxer::getDuration()
{
    uint64_t aTime = 0;
    uint64_t vTime = 0;
    
    if (!isOpen || (!hasVideo() && !hasAudio())){
        return std::chrono::milliseconds(0);
    }
    
    if (hasAudio()){
        aTime = (uint64_t) ((double) fmtCtx->streams[videoStreamIdx]->duration * (double) fmtCtx->streams[videoStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[videoStreamIdx]->time_base.den * 1000.0);
    }
    
     if (hasVideo()){
        vTime = (uint64_t) ((double) fmtCtx->streams[videoStreamIdx]->duration * (double) fmtCtx->streams[videoStreamIdx]->time_base.num / 
                (double) fmtCtx->streams[videoStreamIdx]->time_base.den * 1000.0);
    }
    
    if (aTime > vTime){
        return std::chrono::milliseconds(aTime);
    }
    
    return std::chrono::milliseconds(vTime);
}

size_t Demuxer::getNalSizeBytes(unsigned char* metadata)
{
    size_t nOfBytes = 0;
    size_t nOfBytesMinusOne = 0;

    nOfBytesMinusOne = metadata[N_OF_NAL_SIZE_BYTES_MINUS_ONE_POSITION] & N_OF_NAL_SIZE_BYTES_MINUS_ONE_MASK;
    nOfBytes = nOfBytesMinusOne + 1;

    return nOfBytes;
}

bool Demuxer::isIntra(unsigned char* data)
{
    bool isIntra = false;
    int nalType = 0;

    nalType = data[nalSizeBytes] & NAL_TYPE_MASK;

    if (nalType == IDR_NAL_TYPE || nalType == SEI_NAL_TYPE) {
        isIntra = true;
    }

    return isIntra;
}


