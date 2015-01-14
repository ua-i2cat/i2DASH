/*
 *  CloseWriteWatcher - Class to monitor written files in a directory
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
#include <chrono>
#include <thread>

#include <unistd.h>
#include <sys/stat.h>

CloseWriteWatcher::CloseWriteWatcher()
{
    fd = wd = 0;
    callback = NULL;
    folderPath = "";
}

CloseWriteWatcher::~CloseWriteWatcher()
{
    if (wd > 0 && fd > 0){
        inotify_rm_watch(fd, wd);
        close(fd);
    }
}

bool CloseWriteWatcher::setWatchFolder(std::string folder)
{
    if (testFolder(folder)){
        folderPath = folder;
        return true;
    } 
    
    folderPath = "";
    return false;
}

void CloseWriteWatcher::setCallback(std::function<void (std::string fileName)> funct)
{
    callback = funct;
}

void CloseWriteWatcher::startWatching(int &watch)
{
    int i;
    int length;
    std::string targetFile;
    
    if (folderPath.empty() || callback == NULL){
        return;
    }
    
    fd = inotify_init1(IN_NONBLOCK);
    if ( fd < 0 ) {
        std::cerr << "inotify initialization failed!" << std::endl;
        return;
    }
    
    wd = inotify_add_watch( fd, folderPath.c_str(), IN_CLOSE_WRITE);
    if ( wd < 0 ) {
        std::cerr << "inotify add watch failed!" << std::endl;
        return;
    }
    
    while(watch){
        
        length = read(fd, buffer, EVENT_BUF_LEN); 
        if (length < 0){
            std::this_thread::sleep_for(std::chrono::microseconds(SLEEP_TIME_USEC));
        }
    
        i = 0;
        while (length > 0 && i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];     
            if (event->len > 0) {
                if (event->mask & IN_CLOSE_WRITE) {
                    if (! (event->mask & IN_ISDIR)) {
                        targetFile = event->name;
                        targetFile = folderPath + "/" + targetFile;
                        callback(targetFile);
                    }
                }
                i += EVENT_SIZE + event->len;
            }
        }

    }
    
    inotify_rm_watch(fd, wd);
    close(fd);
    
    wd = fd = 0;
}

bool CloseWriteWatcher::testFolder(std::string folderName)
{   
    struct stat buffer;
    return (stat(folderName.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}


