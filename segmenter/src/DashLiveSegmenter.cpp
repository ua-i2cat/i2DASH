/*
 *  DashLiveSegmenter - Main for generating dash segments from MP4 segments in live
 *  Copyright (C) 2015  Fundació i2CAT, Internet i Innovació digital a Catalunya
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
 *  Authors:  David Cassany <david.cassany@i2cat.net>
 */

#include "CloseWriteWatcher.hh"

#include <iostream>
#include <csignal>
#include <cstdio>
#include <thread>
#include <chrono>

#include "Demuxer.hh"
#include "Frame.hh"
#include "DashVideoSegmenter.hh"
#include "DashAudioSegmenter.hh"
#include "DashSegment.hh"
#include "MP4ToDashConverter.hh"
#include <sys/stat.h>

int run = 1;
MP4ToDashConverter* converter;

void signalHandler( int signum )
{
    run = 0;
}

void produceFile(std::string filePath);

bool testFolder(std::string folderName)
{   
    struct stat buffer;
    return (stat(folderName.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

int main(int argc, char* argv[])
{
    std::string watchPath;
    std::string destinationPath;
    
    if (argc != 3) {
        std::cerr << "Error invalid number of parameters" << std::endl;
        return 1;
    }
    
    watchPath = argv[1];
    destinationPath = argv[2];
    
    if (! testFolder(watchPath)){
        std::cerr << "Invalid watch path" << std::endl;
        return 1;
    }
    
    if (! testFolder(destinationPath)){
        std::cerr << "Invalid destination path" << std::endl;
        return 1;
    }
    
    converter = new MP4ToDashConverter(destinationPath);
    
    CloseWriteWatcher *watch = new CloseWriteWatcher();
    
    watch->setWatchFolder(watchPath);
    watch->setCallback(produceFile);
    
    watch->startWatching(run);
    
    delete watch;
    
    return 0;
};



void produceFile(std::string filePath)
{
    converter->produceFile(filePath);
}
