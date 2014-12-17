/*
 *  DashSegment - DASH segment container class
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

#ifndef _DASH_SEGMENT_HH
#define _DASH_SEGMENT_HH

#include <stdlib.h>
#include <string>

#define SEGMENT_MAX_SIZE 1024*1024 //1MB
#define SEGMENT_DEFAULT_DURATION 1000 //ms

class DashSegment {
    
public:
    /**
    * Class constructor
    * @param outputFile Output file name
    * @param maxSize Segment data max length
    * @param seqNum Segment sequence number
    */ 
    DashSegment(std::string outputFile, size_t maxSize, size_t seqNum);

    /**
    * Class destructor
    */ 
    ~DashSegment();

    /**
    * @return Pointer to segment data
    */ 
    unsigned char* getDataBuffer() {return data;};

    /**
    * @return Segment data length in bytes
    */ 
    size_t getDataLength() {return dataLength;};

    /**
    * @params Segment data length in bytes
    */ 
    void setDataLength(size_t length);

    /**
    * @return Segment sequence number
    */ 
    size_t getSeqNumber(){return seqNumber;};

    /**
    * Writes segment to disk
    */ 
    void writeToDisk();


private:
    unsigned char* data;
    size_t dataLength;
    size_t seqNumber;
    std::string path;

};


#endif
