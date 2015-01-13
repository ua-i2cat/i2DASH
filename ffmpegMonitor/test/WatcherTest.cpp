#include "CloseWriteWatcher.hh"

#include <iostream>
#include <csignal>

int run = 1;

void signalHandler( int signum )
{
    run = 0;
}

void testFunct(std::string fileName){
    std::cout << "this is test funct" << std::endl;
}

int main(int argc, char* argv[])
{
    CloseWriteWatcher *watch = new CloseWriteWatcher();
    
    watch->setWatchFolder("./tmp");
    watch->setCallback(testFunct);
    
    watch->startWatching(run);
    
    delete watch;
};

