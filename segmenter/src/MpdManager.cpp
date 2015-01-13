/*
 *  MpdManager - DASH MPD manager class
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
 *  Authors: Xavier Carol <xavier.carol@i2cat.net>
 */

#include <fstream>
#include <tinyxml2.h>

#include "MpdManager.hh"
#include "MpdSkeleton.hh"

MpdManager::MpdManager()
{
}

MpdManager::~MpdManager()
{
}

bool MpdManager::writeSkeleton(const char* fileName)
{
    std::ofstream outfile(fileName, std::ofstream::out);

    if (outfile.fail())
        return false;

    outfile << MPD_SKELETON;

    if (outfile.good())
        return true;

    return false;
}

bool MpdManager::updateMpd(const char* fileName)
{
    tinyxml2::XMLDocument doc;

    doc.LoadFile(fileName);
    if (doc.ErrorID())
        return false;

    return true;
}
