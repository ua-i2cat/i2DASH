/*
 *  closeWriteWatcher - Class to monitor written files in a directory
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


#ifndef _CLOSE_WRITE_WATCHER_HH
#define _CLOSE_WRITE_WATCHER_HH

#include <string>
#include <functional>
#include <sys/inotify.h>

#define EVENT_SIZE      (sizeof (struct inotify_event))
#define EVENT_BUF_LEN   (1024 * (EVENT_SIZE + 16))
#define SLEEP_TIME_USEC 2500

class CloseWriteWatcher {
public:
    CloseWriteWatcher();
    ~CloseWriteWatcher();
    
    bool setWatchFolder(std::string folder);
    std::string getWatchFolder() {return folderPath;};
    void setCallback(std::function<void (std::string fileName)> funct);
    
    void startWatching(int &watch);
    
    
private:
    bool testFolder(std::string folder);
    
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];
    std::string folderPath;
    std::function<void (std::string fileName)> callback;
};

#endif
