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

#ifndef _MP4_TO_DASH_CONVERTER_HH
#define _MP4_TO_DASH_CONVERTER_HH

#include "Demuxer.hh"
#include "Frame.hh"
#include "DashVideoSegmenter.hh"
#include "DashAudioSegmenter.hh"
#include "DashSegment.hh"
#include "MpdManager.hh"

#define V_ADAPT_SET_ID "0"
#define A_ADAPT_SET_ID "1"
#define VIDEO_CODEC "avc1.42c01e"
#define AUDIO_CODEC "mp4a.40.2"

class MP4ToDashConverter {
public:
    MP4ToDashConverter(std::string destination, std::string mpdLocation);
    ~MP4ToDashConverter();
    
    void produceFile(std::string filePath);
    
private:

    std::string getVideoSegTemplateFromPath(std::string filePath);
    std::string getAudioSegTemplateFromPath(std::string filePath);
    std::string getVideoInitTemplateFromPath(std::string filePath);
    std::string getAudioInitTemplateFromPath(std::string filePath);

    std::string getRepresentationIdFromPath(std::string filePath);
    std::string getMpdNameFromLocation(std::string location);
    int getSeqNumberFromPath(std::string filePath);
    std::string getVideoInitPath(std::string filePath);
    std::string getAudioInitPath(std::string filePath);
    std::string getVideoPath(std::string filePath, size_t ts);
    std::string getAudioPath(std::string filePath, size_t ts);
    
    Demuxer* demux;
    DashVideoSegmenter* vSeg;
    DashAudioSegmenter* aSeg;
    DashSegment* aSegment;
    DashSegment* vSegment;
    MpdManager* mpdManager;
    
    std::string destinationPath;
    std::string mpdPath;
};

#endif
