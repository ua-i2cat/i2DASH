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

/*! It is used to create audio dash segments. It has to be initializaed after creating it using init(). After that, use
    addToSegment() to add audio frames to a segment and finishSegment() to close it */ 

class DashAudioSegmenter {
    
public:
    /**
    * Class constructor
    */
    DashAudioSegmenter();

    /**
    * Class destructor
    */
    ~DashAudioSegmenter();

    /**
    * Init and configure internal structures. It is necessary to execute it before generateInit(), addToSegment() and finishSegment()
    * @param segmentDuration Total segment duration in timeBase units
    * @param timeBase Time base in ticks per second
    * @param timeBase Estimate sample duration (consider sample == frame)
    * @param channels Audio channels
    * @param sampleRate Audio sample rate (in Hz)
    * @param sampleSize Audio bits per sample
    * @return true if succeeded and false if not
    */
    bool init(size_t segmentDuration, size_t timeBase, size_t sampleDuration, size_t channels, size_t sampleRate, size_t sampleSize);
    
    /**
    * Generates init file
    * @param metadata Audio metadata extracted from the media container
    * @param metadataSize Audio matadata size
    * @param segment Destination DashSegment. Each of its attributes must be set to a correct value before executing generateInit()
    * @return true if succeeded and false if not
    * see @DashSegment
    */ 
    bool generateInit(unsigned char* metadata, size_t metadataSize, DashSegment* segment);
    
    /**
    * Add frame to a dash segment
    * @param frame Origin AVCCFrame. Each of its attributes must be set to a correct value before executing addToSegment()
    * @param segment Destination DashSegment. Each of its attributes must be set to a correct value before executing addToSegment()
    * @return true if segment is completed and false if not
    * see @DashSegment
    * see @AVCCFrame
    */ 
    bool addToSegment(AACFrame* frame, DashSegment* segment);

    /**
    * Finish and close a dash segment
    * @param segment Target DashSegment. Each of its attributes must be set to a correct value before executing finishSegment()
    * @return true if segment is completed and false if not
    * see @DashSegment
    * see @AVCCFrame
    */ 
    bool finishSegment(DashSegment* segment);

    /**
    * @return Max segment length in bytes
    */ 
    size_t getMaxSegmentLength(){return MAX_DAT;}; 

private:
    i2ctx* dashContext;
    size_t channels;
    size_t sampleRate;
    size_t sampleSize;
};


#endif
