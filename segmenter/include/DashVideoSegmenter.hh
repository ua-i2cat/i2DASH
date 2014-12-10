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

#ifndef _DASH_VIDEO_SEGMENTER_HH
#define _DASH_VIDEO_SEGMENTER_HH

#include "i2libdash.h"
#include "Frame.hh"
#include "DashSegment.hh"
#include <chrono>

#define DEFAULT_VIDEO_SEG_DURATION 1

class DashVideoSegmenter {
    
public:
    DashVideoSegmenter();
    ~DashVideoSegmenter();

    bool init();
    bool generateInit(AVCCFrame* frame, DashSegment* segment);
    bool addToSegment(AVCCFrame* frame, DashSegment* segment); 

    
private:
    int segDurationInSec;
    int width;
    int height;
    int framerate;
    i2ctx* dashContext;
    std::chrono::milliseconds previousTimestamp;

};


#endif
