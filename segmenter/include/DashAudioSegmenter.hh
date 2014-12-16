/*
 *  DashAudioSegmenter - Audio DASH segmenter for AAC
 *  Copyright (C) 2013  Fundació i2CAT, Internet i Innovació digital a Catalunya
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
 *  Authors: Marc Palau <marc.palau@i2cat.net> 
 */

#ifndef _DASH_AUDIO_SEGMENTER_HH
#define _DASH_AUDIO_SEGMENTER_HH

#include "i2libdash.h"
#include "Frame.hh"
#include "DashSegment.hh"

class DashAudioSegmenter {
    
public:
    DashAudioSegmenter();
    ~DashAudioSegmenter();

    bool init(size_t segmentDuration, size_t timeBase, size_t sampleDuration, size_t channels, size_t sampleRate, size_t sampleSize);
    bool generateInit(unsigned char* metadata, size_t metadataSize, DashSegment* segment);
    bool addToSegment(AACFrame* frame, DashSegment* segment);
    bool finishSegment(DashSegment* segment);
    size_t getMaxSegmentLength(){return MAX_DAT;}; 

private:
    i2ctx* dashContext;
    size_t channels;
    size_t sampleRate;
    size_t sampleSize;
};


#endif
