#include "CloseWriteWatcher.hh"

#include <iostream>
#include <csignal>
#include <cstdio>
#include <thread>
#include <chrono>

int run = 1;

void signalHandler( int signum )
{
    run = 0;
}

void testFunct(std::string fileName){
    std::cout << "this is test funct, found file: " << fileName << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (std::remove(fileName.c_str()) == 0){
        std::cout << "deleted file: " << fileName << std::endl;
    } else {
        std::cout << "Coudn't delete file: " << fileName << std::endl;
    }
}

int main(int argc, char* argv[])
{
    CloseWriteWatcher *watch = new CloseWriteWatcher();
    
    watch->setWatchFolder("./tmp");
    watch->setCallback(testFunct);
    
    watch->startWatching(run);
    
    delete watch;
};


