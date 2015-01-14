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

#include "DashSegment.hh"
#include <fstream>
#include <iostream>

DashSegment::DashSegment(size_t maxSize, size_t seqNum) : dataLength(0), seqNumber(seqNum)
{
    data = new unsigned char[maxSize];
}

DashSegment::~DashSegment()
{
    delete[] data;
}

void DashSegment::setDataLength(size_t length)
{
    dataLength = length;
}

void DashSegment::writeToDisk(std::string path)
{
    const char* p = path.c_str();
    std::ofstream file(p, std::ofstream::binary);
    file.write((char*)data,dataLength);
    file.close();
}

void DashSegment::setTimestamp(size_t ts)
{
    timestamp = ts;
}

