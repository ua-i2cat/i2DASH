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

#include "DashAudioSegmenter.hh"
#include <iostream>


DashAudioSegmenter::DashAudioSegmenter() 
: dashContext(NULL), previousTimestamp(std::chrono::milliseconds(0)), segmentDuration(std::chrono::milliseconds(0)), channels(0), sampleRate(0), sampleSize(0)
{
    
}

bool DashAudioSegmenter::init(std::chrono::milliseconds segmentDuration, size_t channels, size_t sampleRate, size_t sampleSize) 
{
    uint8_t i2error;
    this->channels = channels;
    this->sampleRate = sampleRate;
    this->sampleSize = sampleSize;
    this->segmentDuration = segmentDuration;
    
    i2error = generate_context(&dashContext, AUDIO_TYPE);

    if (i2error != I2OK) {
        return false;
    }

    i2error = fill_audio_context(&dashContext, channels, sampleRate, sampleSize); 

    if (i2error != I2OK) {
        return false;
    }

    set_segment_duration_ms(segmentDuration.count(), &dashContext);
    return true;
}

DashAudioSegmenter::~DashAudioSegmenter()
{
}

bool DashAudioSegmenter::generateInit(unsigned char* metadata, size_t metadataSize, DashSegment* segment)
{
    size_t initSize = 0;

    if (!dashContext) {
        return false;
    }

    initSize = init_audio_handler(metadata, metadataSize, segment->getDataBuffer(), &dashContext);

    if (initSize == 0) {
        return false;
    }

    segment->setDataLength(initSize);

    return true;
}

bool DashAudioSegmenter::addToSegment(AACFrame* frame, DashSegment* segment)
{
    std::chrono::milliseconds sampleDuration;
    std::chrono::milliseconds frameTimestamp = frame->getPresentationTime();
    size_t segmentSize = 0;

    if (!frame || !segment || frame->getDataLength() <= 0 || !dashContext) {
        return false;
    }

    //TODO: first frame duration
    sampleDuration = frameTimestamp - previousTimestamp;
    previousTimestamp = frameTimestamp; 

    segmentSize = add_sample(frame->getDataBuffer(), frame->getDataLength(), sampleDuration.count(), 
                             frameTimestamp.count(), AUDIO_TYPE, segment->getDataBuffer(), 1, &dashContext);

    if (segmentSize <= I2ERROR_MAX) {
        return false;
    }

    segment->setDataLength(segmentSize);
    return true;
}
