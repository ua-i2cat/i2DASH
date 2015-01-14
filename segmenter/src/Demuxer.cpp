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

#define BITS_PER_BYTE 8
#define TO_MILLISECONDS 1000

Demuxer::Demuxer(size_t vTime, size_t aTime): fmtCtx(NULL), audioStream(NULL), 
                    videoStream(NULL), videoStreamIdx(-1), 
                    audioStreamIdx(-1), framesCounter(0), isOpen(false),
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
    
    if (isOpen) {
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
    audioStream = NULL;
    videoStream = NULL;
    videoStreamIdx = -1;
    audioStreamIdx = -1;
}

bool Demuxer::isInputOpen()
{
    return isOpen;
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
    bool streamsFound = false;

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
    
    streamsFound = findVideoStream();
    streamsFound |= findAudioStream();
    
    return streamsFound;
}

bool Demuxer::findVideoStream()
{
    int videoBitRate, width, height;
    float fps;
    
    if (!isOpen){
        return false;
    }
    
    videoStreamIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    if (videoStreamIdx < 0 || videoStreamIdx == audioStreamIdx) {
        videoStreamIdx = -1;
        return false;
    }
        
    videoStream = fmtCtx->streams[videoStreamIdx];
    
    videoBitRate = videoStream->codec->bit_rate;
    width = videoStream->codec->width;
    height = videoStream->codec->height;
    fps = (float) videoStream->avg_frame_rate.num / (float) videoStream->avg_frame_rate.den;

    if (fps <= 0 || videoBitRate <= 0 || width <= 0 || height <= 0) {
        videoStreamIdx = -1;
        return false;
    }

    if (!validVideoCodec()) {
        videoStreamIdx = -1;
        return false;
    }

    if (videoStream->codec->extradata_size <= 0) {
        videoStreamIdx = -1;
        return false;
    }

    nalSizeBytes = getNalSizeBytes(videoStream->codec->extradata);   
    
    return true;
}

bool Demuxer::findAudioStream()
{
    int audioBitRate, channels, sampleRate, bitsPerSample;
    
    if (!isOpen){
        return false;
    }
    
    audioStreamIdx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioStreamIdx < 0 || videoStreamIdx == audioStreamIdx){
        audioStreamIdx = -1;
        return false;
    }
    
    audioStream = fmtCtx->streams[audioStreamIdx];
    
    audioBitRate = audioStream->codec->bit_rate;
    channels = audioStream->codec->channels;
    sampleRate = audioStream->codec->sample_rate;
    bitsPerSample = av_get_bytes_per_sample(audioStream->codec->sample_fmt) * BITS_PER_BYTE; //Bytes to bits;
    
    if (channels <= 0 || sampleRate <= 0 || audioBitRate <= 0 || bitsPerSample <= 0) {
        audioStreamIdx = -1;
        return false;
    }
    
    if (!validAudioCodec()){
        audioStreamIdx = -1;
        return false;
    }
    
    if (audioStream->codec->extradata_size <= 0) {
        audioStreamIdx = -1;
        return false;
    }
    
    return true;
}

bool Demuxer::validVideoCodec()
{
    if (!isOpen || videoStreamIdx < 0){
        return false;
    }
    
    if (fmtCtx->streams[videoStreamIdx]->codec->codec_id != CODEC_ID_H264) {
        std::cerr << "Invalid video codec. IT must be H264" << std::endl;
        return false;
    }
    
    return true;
}

bool Demuxer::validAudioCodec()
{
    if (!isOpen || audioStreamIdx < 0) { 
        return false;
    }
    
    if (fmtCtx->streams[audioStreamIdx]->codec->codec_id != CODEC_ID_AAC) {
        std::cerr << "Invalid audio codec. It must be AAC" << std::endl;
        return false;
    }
       
    return true;
}


bool Demuxer::hasVideo()
{
    if (!isOpen || fmtCtx == NULL) {
        return false;
    }
    
    return videoStreamIdx >= 0;
}

bool Demuxer::hasAudio()
{
    if (!isOpen || fmtCtx == NULL) {
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
    
    if (gotFrame < 0 || pkt.size <= 0) {
        return NULL;
    }
    
    videoFrame->clearFrame();
    audioFrame->clearFrame();

    if (pkt.stream_index == videoStreamIdx) {
        videoFrame->setDataBuffer(pkt.data, pkt.size);

        videoFrame->setIntra(isIntra(pkt.data));
        
        videoFrame->setPresentationTime(pkt.pts + vStartTime);
        videoFrame->setDecodeTime(pkt.dts + vStartTime);
        videoFrame->setDuration(pkt.duration);

        if (pkt.duration < 0) {
            audioFrame->setDuration(getVideoSampleDuration());
            std::cerr << "Negative duration, setting estimate frame duration " << std::endl;
        }

        return videoFrame;
    }

    if (pkt.stream_index == audioStreamIdx) {
        audioFrame->setDataBuffer(pkt.data, pkt.size);
        
        audioFrame->setPresentationTime(pkt.pts + aStartTime);
        audioFrame->setDecodeTime(pkt.dts + aStartTime);
        audioFrame->setDuration(pkt.duration);

        if (pkt.duration < 0) {
            audioFrame->setDuration(getAudioSampleDuration());
            std::cerr << "Negative duration, setting estimate frame duration: " << std::endl;
        }

        return audioFrame;
    } 
    
    return NULL;
}

size_t Demuxer::getAudioDuration()
{   
    if (!isOpen || !hasAudio()) {
        return 0;
    }
    
    return audioStream->duration;;
}

size_t Demuxer::getVideoDuration()
{    
    if (!isOpen || !hasVideo()) {
        return 0;
    }
    
    return videoStream->duration;
}

size_t Demuxer::getAudioSampleDuration()
{    
    if (!isOpen || !hasAudio()) {
        return 0;
    }
    
    return audioStream->duration / audioStream->nb_frames;
}

size_t Demuxer::getVideoSampleDuration()
{   
    if (!isOpen || !hasVideo()) {
        return 0;
    }
    
    return videoStream->duration / videoStream->nb_frames;
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

float Demuxer::getFPS()
{
    return (float) videoStream->avg_frame_rate.num / (float) videoStream->avg_frame_rate.den;
}

size_t Demuxer::getAudioBitsPerSample()
{
    return av_get_bytes_per_sample(audioStream->codec->sample_fmt) * BITS_PER_BYTE;
}

size_t Demuxer::getAudioTimeBase()
{
    return audioStream->time_base.den / audioStream->time_base.num;
}

size_t Demuxer::getVideoTimeBase()
{
    return videoStream->time_base.den / videoStream->time_base.num;
}
