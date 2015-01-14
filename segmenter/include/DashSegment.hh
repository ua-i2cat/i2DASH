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

/*! It represents a dash segment. It contains a buffer with the segment data (it allocates data) and its length. Moreover, it contains the
    segment sequence number and the output file name (in order to write the segment to disk) */ 

class DashSegment {
    
public:
    /**
    * Class constructor
    * @param maxSize Segment data max length
    * @param seqNum Segment sequence number
    */ 
    DashSegment(size_t maxSize, size_t seqNum);

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
    * @return Segment sequence number
    */ 
    size_t getTimestamp(){return timestamp;};

    /**
    * @return Segment sequence number
    */ 
    void setTimestamp(size_t ts);

    /**
    * Writes segment to disk
    * @params Path to write
    */ 
    void writeToDisk(std::string path);


private:
    unsigned char* data;
    size_t dataLength;
    size_t seqNumber;
    size_t timestamp;
    std::string path;

};


#endif
