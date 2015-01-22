/*
 *  MP4ToDashConverter - Class that converts an MP4 segment to a Dash segment
 * 
 *  Copyright (C) 2015  Fundació i2CAT, Internet i Innovació digital a Catalunya
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
 */

#include "MP4ToDashConverter.hh"

#include <iostream>
#include <string>
#include <cstdio>

MP4ToDashConverter::MP4ToDashConverter(std::string destination)
{
    demux = new Demuxer();
    vSeg = new DashVideoSegmenter();
    aSeg = new DashAudioSegmenter();
    vSegment = new DashSegment(vSeg->getMaxSegmentLength());
    aSegment = new DashSegment(aSeg->getMaxSegmentLength());
    mpdManager = new MpdManager();
    destinationPath = destination;
    mpdPath = "";
}

MP4ToDashConverter::~MP4ToDashConverter()
{
    delete demux;
    delete vSeg;
    delete aSeg;
    delete vSegment;
    delete aSegment;
    delete mpdManager;
}

void MP4ToDashConverter::setMpdLocation(std::string mpdLocation)
{
    mpdManager->setLocation(mpdLocation);
    mpdPath = destinationPath + getMpdNameFromLocation(mpdLocation);
} 

std::string MP4ToDashConverter::getVideoSegTemplateFromPath(std::string filePath)
{
    size_t e;
    std::string base;
    std::string cut;
    std::string templ;

    try {
        e = filePath.find_last_of("/");
        base = filePath.substr(e + 1, filePath.length() - e);
        
        e = base.find_last_of("_");
        cut = base.substr(0, e);

        e = cut.find_last_of("_");
        templ = cut.substr(0, e);

        templ += "_$RepresentationID$_$Time$.m4v";
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return templ;
}

std::string MP4ToDashConverter::getAudioSegTemplateFromPath(std::string filePath)
{
    size_t e;
    std::string base;
    std::string cut;
    std::string templ;

    try {
        e = filePath.find_last_of("/");
        base = filePath.substr(e + 1, filePath.length() - e);
        
        e = base.find_last_of("_");
        cut = base.substr(0, e);

        e = cut.find_last_of("_");
        templ = cut.substr(0, e);
        
        templ += "_$RepresentationID$_$Time$.m4a";
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return templ;
}

std::string MP4ToDashConverter::getVideoInitTemplateFromPath(std::string filePath)
{
    size_t e;
    std::string base;
    std::string cut;
    std::string templ;

    try {
        e = filePath.find_last_of("/");
        base = filePath.substr(e + 1, filePath.length() - e);
        
        e = base.find_last_of("_");
        cut = base.substr(0, e);

        e = cut.find_last_of("_");
        templ = cut.substr(0, e);
    
        templ += "_$RepresentationID$_init.m4v";
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return templ;
}

std::string MP4ToDashConverter::getAudioInitTemplateFromPath(std::string filePath)
{
    size_t e;
    std::string base;
    std::string cut;
    std::string templ;

    try {
        e = filePath.find_last_of("/");
        base = filePath.substr(e + 1, filePath.length() - e);
        
        e = base.find_last_of("_");
        cut = base.substr(0, e);

        e = cut.find_last_of("_");
        templ = cut.substr(0, e);
        
        templ += "_$RepresentationID$_init.m4a";
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return templ;
}


int MP4ToDashConverter::getSeqNumberFromPath(std::string filePath)
{
    int seqNumber = -1;

    try {
        size_t b = filePath.find_last_of("_");
        size_t e = filePath.find_last_of(".");

        std::string stringSequenceNumber = filePath.substr(b+1,e-b-1);
        seqNumber = stoi(stringSequenceNumber);
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return seqNumber;
}

std::string MP4ToDashConverter::getRepresentationIdFromPath(std::string filePath)
{
    size_t e;
    std::string cut;
    std::string id;

    try {
        e = filePath.find_last_of("_");
        cut = filePath.substr(0,e);
        
        e = cut.find_last_of("_");
        id = cut.substr(e + 1, cut.length() - e);
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return id;
}

std::string MP4ToDashConverter::getMpdNameFromLocation(std::string location)
{
    std::string name;
    size_t e;

    try {
        e = location.find_last_of("/");
        name = location.substr(e, location.length() - e);
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return name;
}


std::string MP4ToDashConverter::getVideoInitPath(std::string filePath)
{
    std::string path;
    size_t e;

    try {
        e = filePath.find_last_of("_");
        path = filePath.substr(0,e) + "_init.m4v";
        e = path.find_last_of("/");
        if (e != std::string::npos) {
            path = destinationPath + path.substr(e);
        }
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return path;
}

std::string MP4ToDashConverter::getAudioInitPath(std::string filePath)
{
    std::string path;
    size_t e;

    try {
        e = filePath.find_last_of("_");
        path = filePath.substr(0,e) + "_init.m4a";
        e = path.find_last_of("/");
        if (e != std::string::npos){
            path = destinationPath + path.substr(e);
        }
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return path;
}

std::string MP4ToDashConverter::getVideoPath(std::string filePath, size_t ts)
{
    std::string path;
    std::string timestamp;
    size_t e;

    try {
        timestamp = std::to_string(ts);
        e = filePath.find_last_of("_");
        path = filePath.substr(0,e) + "_" + timestamp + ".m4v";
        e = path.find_last_of("/");
        if (e != std::string::npos){
            path = destinationPath + path.substr(e);
        }
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return path;
}

std::string MP4ToDashConverter::getAudioPath(std::string filePath, size_t ts)
{
    std::string path;
    std::string timestamp = std::to_string(ts);
    size_t e;

    try {
        e = filePath.find_last_of("_");
        path = filePath.substr(0,e) + "_" + timestamp + ".m4a";
        e = path.find_last_of("/");
        if (e != std::string::npos){
            path = destinationPath + path.substr(e);
        }
    } catch ( const std::out_of_range& oor ) {
        std::cerr << "Out of Range error: " << oor.what() << std::endl;
    }

    return path;
}

void MP4ToDashConverter::closeVideoSegment(std::string filePath)
{
    vSegment->writeToDisk(getVideoPath(filePath, vSegment->getTimestamp()));
           
    if (!mpdManager->updateAdaptationSetTimestamp(V_ADAPT_SET_ID, vSegment->getTimestamp(), demux->getVideoDuration())) {
        std::cerr << "Error updating video timestamp. Adaptation set does not exist" << std::endl;
    }

    mpdManager->writeToDisk(mpdPath.c_str());
}

void MP4ToDashConverter::closeAudioSegment(std::string filePath)
{
    aSegment->writeToDisk(getAudioPath(filePath, aSegment->getTimestamp()));

    if (!mpdManager->updateAdaptationSetTimestamp(A_ADAPT_SET_ID, aSegment->getTimestamp(), demux->getAudioDuration())) {
        std::cerr << "Error updating audio timestamp. Adaptation set does not exist" << std::endl;
    }

    mpdManager->writeToDisk(mpdPath.c_str());
}

void MP4ToDashConverter::produceFile(std::string filePath)
{   
    AVCCFrame* videoFrame;
    AACFrame* audioFrame;
    Frame* frame;
    int gotFrame = 0;
    int seqNumber = -1;
    std::string representationId;
    int bandwidth;

    if (mpdPath.empty()) {
        std::cerr << "MPD path has not been set correctly. Impossible to produce DASH segment without a valid mpd location" << std::endl;
        return;
    }

    seqNumber = getSeqNumberFromPath(filePath);
    representationId = getRepresentationIdFromPath(filePath);
    bandwidth = atoi(representationId.c_str())*1000;

    if (!demux->openInput(filePath)){
        std::cerr << "Error openInput" << std::endl;
        return;
    }
    
    if (!demux->findStreams()){
        std::cerr << "Error findStreams" << std::endl;
        return;
    }

    if (demux->hasVideo()) {
        if (!vSeg->init(demux->getVideoDuration(), demux->getVideoTimeBase(), 
                        demux->getVideoSampleDuration(), 
                        demux->getWidth(), demux->getHeight(), demux->getFPS())) {
            std::cerr << "Error initializing Video Segmenter" << std::endl;
            return;
        }
       
        if (!vSegment->isEmpty()) {
            std::cerr << "Error no empty segment" << std::endl;
            return;
        }
        
        vSegment->setSeqNumber(seqNumber);
        
        if (!vSeg->generateInit(demux->getVideoExtraData(), demux->getVideoExtraDataLength(), vSegment)) {
            std::cerr << "Error constructing video init" << std::endl;
            return;
        }

        mpdManager->setMinBufferTime((demux->getVideoDuration()/demux->getVideoTimeBase())*(MAX_SEGMENTS_IN_MPD/2));
        mpdManager->setMinimumUpdatePeriod(demux->getVideoDuration()/demux->getVideoTimeBase());
        mpdManager->setTimeShiftBufferDepth((demux->getVideoDuration()/demux->getVideoTimeBase())*MAX_SEGMENTS_IN_MPD);
        mpdManager->updateVideoAdaptationSet(V_ADAPT_SET_ID, demux->getVideoTimeBase(),
                                                       getVideoSegTemplateFromPath(filePath),
                                                       getVideoInitTemplateFromPath(filePath));
        mpdManager->updateVideoRepresentation(V_ADAPT_SET_ID, representationId, VIDEO_CODEC, 
                                  demux->getWidth(), demux->getHeight(), bandwidth, demux->getFPS());
        
        //TODO:: think about comparing existing init with this one in order to optimize hdd writing
        vSegment->writeToDisk(getVideoInitPath(filePath));
        mpdManager->writeToDisk(mpdPath.c_str());
        vSegment->clear();
    }

    if (demux->hasAudio()) {
        if (!aSeg->init(demux->getAudioDuration(), demux->getAudioTimeBase(), demux->getAudioSampleDuration(), 
                        demux->getAudioChannels(), demux->getAudioSampleRate(), demux->getAudioBitsPerSample())) {
            std::cerr << "Error initializing Audio Segmenter" << std::endl;
            return;
        }

        if (!aSegment->isEmpty()) {
            std::cerr << "Error no empty segment" << std::endl;
            return;
        }

        aSegment->setSeqNumber(seqNumber);
        
        if (!aSeg->generateInit(demux->getAudioExtraData(), demux->getAudioExtraDataLength(), aSegment)) {
            std::cerr << "Error constructing audio DashSegment objects" << std::endl;
            return;
        }
        
        mpdManager->setMinBufferTime((demux->getAudioDuration()/demux->getAudioTimeBase())*(MAX_SEGMENTS_IN_MPD/2));
        mpdManager->setMinimumUpdatePeriod(demux->getAudioDuration()/demux->getAudioTimeBase());
        mpdManager->setTimeShiftBufferDepth((demux->getAudioDuration()/demux->getAudioTimeBase())*MAX_SEGMENTS_IN_MPD);
        mpdManager->updateAudioAdaptationSet(A_ADAPT_SET_ID, demux->getAudioTimeBase(),
                                                       getAudioSegTemplateFromPath(filePath),
                                                       getAudioInitTemplateFromPath(filePath));
        mpdManager->updateAudioRepresentation(A_ADAPT_SET_ID, representationId, AUDIO_CODEC, 
                                       demux->getAudioSampleRate(), 0, demux->getAudioChannels());
        
        //TODO:: think about comparing existing init with this one in order to optimize hdd writing
        aSegment->writeToDisk(getAudioInitPath(filePath));
        mpdManager->writeToDisk(mpdPath.c_str());
        aSegment->clear();
    }

    while (gotFrame >= 0) {

        frame = demux->readFrame(gotFrame);

        if ((videoFrame = dynamic_cast<AVCCFrame*>(frame)) != NULL && vSeg->addToSegment(videoFrame, vSegment)) {
            closeVideoSegment(filePath);
        }

        if ((audioFrame = dynamic_cast<AACFrame*>(frame)) != NULL && aSeg->addToSegment(audioFrame, aSegment)) {
            closeAudioSegment(filePath);
        }
    }

    if (demux->hasVideo() && vSeg->finishSegment(vSegment)) {
        closeVideoSegment(filePath);
    }

    if (demux->hasAudio() && aSeg->finishSegment(aSegment)) {
        closeAudioSegment(filePath);
    }
    
    if (std::remove(filePath.c_str()) != 0){
        std::cerr << "Coudn't delete file: " << filePath << std::endl;
    }
    
    demux->closeInput();
    vSegment->clear();
    aSegment->clear();
}