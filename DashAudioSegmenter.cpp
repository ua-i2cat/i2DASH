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


DashAudioSegmenter::DashAudioSegmenter(int segmentDurationSeconds) 
: segDurationInSec(segmentDurationSeconds), dashContext(NULL), fSampleRate(0)
{
}

bool DashAudioSegmenter::init() 
{
    uint8_t i2error;

    i2error = context_initializer(&dashContext, AUDIO_TYPE);

    if (i2error == I2ERROR_MEDIA_TYPE) {
        return false;
    }

    set_segment_duration(segDurationInSec, &dashContext);
    return true;
}

DashAudioSegmenter::~DashAudioSegmenter()
{
}

size_t DashAudioSegmenter::generateInit(unsigned char* metadata, size_t metadataSize, unsigned char* initBuffer) 
{
    size_t initSize = 0;

    if (!dashContext) {
        return initSize;
    }

    initSize = init_audio_handler(metadata, metadataSize, initBuffer, &dashContext);

    return initSize;
}

void DashAudioSegmenter::setSampleRate(unsigned sampleRate)
{
    fSampleRate = sampleRate;
}