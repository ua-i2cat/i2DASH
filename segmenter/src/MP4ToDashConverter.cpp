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
 */

#include "MP4ToDashConverter.hh"

#include <iostream>
#include <string>
#include <cstdio>

MP4ToDashConverter::MP4ToDashConverter(std::string destination, std::string mpdLocation)
{
    demux = new Demuxer();
    vSeg = new DashVideoSegmenter();
    aSeg = new DashAudioSegmenter();
    vSegment = new DashSegment(vSeg->getMaxSegmentLength());
    aSegment = new DashSegment(aSeg->getMaxSegmentLength());
    mpdManager = new MpdManager();
    destinationPath = destination;

    mpdManager->getMpd()->setLocation(mpdLocation);
    mpdPath = destinationPath + getMpdNameFromLocation(mpdLocation);
    mpdManager->getMpd()->setMinBufferTime(MIN_BUFFER_TIME);
    mpdManager->getMpd()->setSuggestedPresentationDelay(PRESENTATION_DELAY);
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

std::string MP4ToDashConverter::getVideoSegTemplateFromPath(std::string filePath)
{
    size_t e;
    std::string base;
    std::string cut;
    std::string templ;

    e = filePath.find_last_of("/");
    base = filePath.substr(e + 1, filePath.length() - e);
    
    e = base.find_last_of("_");
    cut = base.substr(0, e);

    e = cut.find_last_of("_");
    templ = cut.substr(0, e);
    
    templ += "_$RepresentationID$_$Time$.m4v";
    return templ;
}

std::string MP4ToDashConverter::getAudioSegTemplateFromPath(std::string filePath)
{
    size_t e;
    std::string base;
    std::string cut;
    std::string templ;

    e = filePath.find_last_of("/");
    base = filePath.substr(e + 1, filePath.length() - e);
    
    e = base.find_last_of("_");
    cut = base.substr(0, e);

    e = cut.find_last_of("_");
    templ = cut.substr(0, e);
    
    templ += "_$RepresentationID$_$Time$.m4a";
    return templ;
}

std::string MP4ToDashConverter::getVideoInitTemplateFromPath(std::string filePath)
{
    size_t e;
    std::string base;
    std::string cut;
    std::string templ;

    e = filePath.find_last_of("/");
    base = filePath.substr(e + 1, filePath.length() - e);
    
    e = base.find_last_of("_");
    cut = base.substr(0, e);

    e = cut.find_last_of("_");
    templ = cut.substr(0, e);
    
    templ += "_$RepresentationID$_init.m4v";
    return templ;
}

std::string MP4ToDashConverter::getAudioInitTemplateFromPath(std::string filePath)
{
    size_t e;
    std::string base;
    std::string cut;
    std::string templ;

    e = filePath.find_last_of("/");
    base = filePath.substr(e + 1, filePath.length() - e);
    
    e = base.find_last_of("_");
    cut = base.substr(0, e);

    e = cut.find_last_of("_");
    templ = cut.substr(0, e);
    
    templ += "_$RepresentationID$_init.m4a";
    return templ;
}


int MP4ToDashConverter::getSeqNumberFromPath(std::string filePath)
{
    int seqNumber = -1;

    size_t b = filePath.find_last_of("_");
    size_t e = filePath.find_last_of(".");

    std::string stringSequenceNumber = filePath.substr(b+1,e-b-1);
    seqNumber = stoi(stringSequenceNumber);

    return seqNumber;
}

std::string MP4ToDashConverter::getRepresentationIdFromPath(std::string filePath)
{
    size_t e;
    std::string cut;
    std::string id;

    e = filePath.find_last_of("_");
    cut = filePath.substr(0,e);
    
    e = cut.find_last_of("_");
    id = cut.substr(e + 1, cut.length() - e);

    return id;
}

std::string MP4ToDashConverter::getMpdNameFromLocation(std::string location)
{
    std::string name;

    size_t e = location.find_last_of("/");
    name = location.substr(e, location.length() - e);
    return name;
}


std::string MP4ToDashConverter::getVideoInitPath(std::string filePath)
{
    std::string path;
    size_t e = filePath.find_last_of("_");
    path = filePath.substr(0,e) + "_init.m4v";
    e = path.find_last_of("/");
    if (e != std::string::npos){
        path = destinationPath + path.substr(e);
    }

    return path;
}

std::string MP4ToDashConverter::getAudioInitPath(std::string filePath)
{
    std::string path;
    size_t e = filePath.find_last_of("_");
    path = filePath.substr(0,e) + "_init.m4a";
    e = path.find_last_of("/");
    if (e != std::string::npos){
        path = destinationPath + path.substr(e);
    }

    return path;
}

std::string MP4ToDashConverter::getVideoPath(std::string filePath, size_t ts)
{
    std::string path;
    std::string timestamp = std::to_string(ts);
    size_t e = filePath.find_last_of("_");
    path = filePath.substr(0,e) + "_" + timestamp + ".m4v";
    e = path.find_last_of("/");
    if (e != std::string::npos){
        path = destinationPath + path.substr(e);
    }

    return path;
}

std::string MP4ToDashConverter::getAudioPath(std::string filePath, size_t ts)
{
    std::string path;
    std::string timestamp = std::to_string(ts);
    size_t e = filePath.find_last_of("_");
    path = filePath.substr(0,e) + "_" + timestamp + ".m4a";
    e = path.find_last_of("/");
    if (e != std::string::npos){
        path = destinationPath + path.substr(e);
    }

    return path;
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

    seqNumber = getSeqNumberFromPath(filePath);
    representationId = getRepresentationIdFromPath(filePath);
    bandwidth = atoi(representationId.c_str())*1000;

    if (!demux || !vSeg || !aSeg || !vSegment || !aSegment) {
        std::cerr << "Error constructing objects" << std::endl;
        exit(1);
    }
    
    if (!demux->openInput(filePath)){
        std::cerr << "Error openInput" << std::endl;
        exit(1);
    }
    
    if (!demux->findStreams()){
        std::cerr << "Error findStreams" << std::endl;
        exit(1);
    }

    if (demux->hasVideo()) {
        if (!vSeg->init(demux->getVideoDuration(), demux->getVideoTimeBase(), 
                        demux->getVideoSampleDuration(), 
                        demux->getWidth(), demux->getHeight(), demux->getFPS())) {
            std::cerr << "Error initializing Video Segmenter" << std::endl;
            exit(1);
        }
       
        if (vSegment->isEmpty()) {
            vSegment->setSeqNumber(seqNumber);
        } else {
            std::cerr << "Error no empty segment" << std::endl;
            exit(1);
        }
        
        if (!vSeg->generateInit(demux->getVideoExtraData(), demux->getVideoExtraDataLength(), vSegment)) {
            std::cerr << "Error constructing video init" << std::endl;
            exit(1);
        }

        mpdManager->getMpd()->setMinimumUpdatePeriod(demux->getVideoDuration()/demux->getVideoTimeBase());
        mpdManager->getMpd()->setTimeShiftBufferDepth((demux->getVideoDuration()/demux->getVideoTimeBase())*MAX_SEGMENTS_IN_MPD);
        mpdManager->getMpd()->updateVideoAdaptationSet(V_ADAPT_SET_ID, demux->getVideoTimeBase(),
                                                       getVideoSegTemplateFromPath(filePath),
                                                       getVideoInitTemplateFromPath(filePath), 
                                                       demux->getVideoDuration());
        mpdManager->getMpd()->updateVideoRepresentation(V_ADAPT_SET_ID, representationId, VIDEO_CODEC, 
                                  demux->getWidth(), demux->getHeight(), bandwidth, demux->getFPS());
        vSegment->writeToDisk(getVideoInitPath(filePath));
        mpdManager->getMpd()->writeToDisk(mpdPath.c_str());
        vSegment->clear();
    }

    if (demux->hasAudio()) {
        if (!aSeg->init(demux->getAudioDuration(), demux->getAudioTimeBase(), demux->getAudioSampleDuration(), 
                        demux->getAudioChannels(), demux->getAudioSampleRate(), demux->getAudioBitsPerSample())) {
            std::cerr << "Error initializing Audio Segmenter" << std::endl;
            exit(1);
        }

        if (aSegment->isEmpty()) {
            aSegment->setSeqNumber(seqNumber);
        } else {
            std::cerr << "Error no empty segment" << std::endl;
            exit(1);
        }
        
        if (!aSeg->generateInit(demux->getAudioExtraData(), demux->getAudioExtraDataLength(), aSegment)) {
            std::cerr << "Error constructing audio DashSegment objects" << std::endl;
            exit(1);
        }
        
        mpdManager->getMpd()->setMinimumUpdatePeriod(demux->getVideoDuration()/demux->getVideoTimeBase());
        mpdManager->getMpd()->setTimeShiftBufferDepth((demux->getVideoDuration()/demux->getVideoTimeBase())*MAX_SEGMENTS_IN_MPD);
        mpdManager->getMpd()->updateAudioAdaptationSet(A_ADAPT_SET_ID, demux->getAudioTimeBase(),
                                                       getAudioSegTemplateFromPath(filePath),
                                                       getAudioInitTemplateFromPath(filePath),  
                                                       demux->getAudioDuration());
        mpdManager->getMpd()->updateAudioRepresentation(A_ADAPT_SET_ID, representationId, AUDIO_CODEC, 
                                       demux->getAudioSampleRate(), 0, demux->getAudioChannels());
        mpdManager->getMpd()->writeToDisk(mpdPath.c_str());
        aSegment->clear();
    }

    while (gotFrame >= 0){

        frame = demux->readFrame(gotFrame);

        if ((videoFrame = dynamic_cast<AVCCFrame*>(frame)) != NULL && vSeg->addToSegment(videoFrame, vSegment)) {
            vSegment->writeToDisk(getVideoPath(filePath, vSegment->getTimestamp()));
            
            if (!mpdManager->getMpd()->updateAdaptationSetTimestamp(V_ADAPT_SET_ID, vSegment->getTimestamp())) {
                std::cerr << "Error updating video timestamp. Adaptation set does not exist" << std::endl;
            }

            mpdManager->getMpd()->writeToDisk(mpdPath.c_str());
        }

        if ((audioFrame = dynamic_cast<AACFrame*>(frame)) != NULL && aSeg->addToSegment(audioFrame, aSegment)) {
            aSegment->writeToDisk(getAudioPath(filePath, aSegment->getTimestamp()));

            if (!mpdManager->getMpd()->updateAdaptationSetTimestamp(A_ADAPT_SET_ID, aSegment->getTimestamp())) {
                std::cerr << "Error updating audio timestamp. Adaptation set does not exist" << std::endl;
            }

            mpdManager->getMpd()->writeToDisk(mpdPath.c_str());
        }
    }

    if (demux->hasVideo() && vSeg->finishSegment(vSegment)) {
        vSegment->writeToDisk(getVideoPath(filePath, vSegment->getTimestamp()));
        
        if (!mpdManager->getMpd()->updateAdaptationSetTimestamp(V_ADAPT_SET_ID, vSegment->getTimestamp())) {
            std::cerr << "Error updating video timestamp. Adaptation set does not exist" << std::endl;
        }
        
        mpdManager->getMpd()->writeToDisk(mpdPath.c_str());
    }

    if (demux->hasAudio() && aSeg->finishSegment(aSegment)) {
        aSegment->writeToDisk(getAudioPath(filePath, aSegment->getTimestamp()));
        
        if (!mpdManager->getMpd()->updateAdaptationSetTimestamp(A_ADAPT_SET_ID, aSegment->getTimestamp())) {
            std::cerr << "Error updating audio timestamp. Adaptation set does not exist" << std::endl;
        }
        
        mpdManager->getMpd()->writeToDisk(mpdPath.c_str());
    }
    
    if (std::remove(filePath.c_str()) != 0){
        std::cout << "Coudn't delete file: " << filePath << std::endl;
    }
    
    demux->closeInput();
    vSegment->clear();
    aSegment->clear();
}