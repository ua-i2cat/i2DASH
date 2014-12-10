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

class DashSegment {
    
public:
    DashSegment(std::string outputFile);
    ~DashSegment();

    unsigned char* getDataBuffer() {return data;};
    size_t getDataLength() {return dataLength;};
    void setDataLength(size_t length);
    void writeToDisk();


private:
    unsigned char* data;
    size_t dataLength;
    std::string path;

};


#endif
