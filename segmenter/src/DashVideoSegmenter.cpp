/*
 *  DashVideoSegmenter - Video DASH segmenter for H264 AVCC format
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

#include "DashVideoSegmenter.hh"
#include <iostream>

DashVideoSegmenter::DashVideoSegmenter() 
: dashContext(NULL), previousTimestamp(std::chrono::milliseconds(0)), segmentDuration(std::chrono::milliseconds(0)),  width(0), height(0), framerate(0)
{
}

bool DashVideoSegmenter::init(std::chrono::milliseconds segmentDuration, size_t width, size_t height, size_t framerate) 
{
    uint8_t i2error;

    this->width = width;
    this->height = height;
    this->framerate = framerate;
    this->segmentDuration = segmentDuration;

    i2error = generate_context(&dashContext, VIDEO_TYPE);

    if (i2error != I2OK) {
        return false;
    }

    i2error = fill_video_context(&dashContext, width, height, framerate);

    if (i2error != I2OK) {
        return false;
    }

    set_segment_duration_ms(segmentDuration.count(), &dashContext);
    return true;
}

DashVideoSegmenter::~DashVideoSegmenter()
{
}

bool DashVideoSegmenter::generateInit(unsigned char *metadata, size_t metadataSize, DashSegment* segment) 
{
    size_t initSize = 0;

    if (!dashContext) {
        return false;
    }

    initSize = new_init_video_handler(metadata, metadataSize, segment->getDataBuffer(), &dashContext);

    if (initSize == 0) {
        return false;
    }

    segment->setDataLength(initSize);

    return true;
}

bool DashVideoSegmenter::addToSegment(AVCCFrame* frame, DashSegment* segment) 
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
                             frameTimestamp.count(), VIDEO_TYPE, segment->getDataBuffer(), frame->isIntra(), &dashContext);

    if (segmentSize <= I2ERROR_MAX) {
        return false;
    }

    segment->setDataLength(segmentSize);
    return true;
}

bool DashVideoSegmenter::finishSegment(DashSegment* segment) 
{
    size_t segmentSize = 0;

    std::cout << "Segmend data size in finish: " << dashContext->ctxvideo->segment_data_size << std::endl;

    if (dashContext->ctxvideo->segment_data_size <= 0) {
        return false;
    }

    segmentSize = finish_segment(VIDEO_TYPE, segment->getDataBuffer(), &dashContext);

    if (segmentSize <= I2ERROR_MAX) {
        return false;
    }

    segment->setDataLength(segmentSize);
    return true;
}

