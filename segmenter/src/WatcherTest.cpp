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

std::string destinationPath = "./dash";

int getSeqNumberFromPath(std::string filePath);
std::string getVideoInitPath(std::string filePath);
std::string getAudioInitPath(std::string filePath);
std::string getVideoPath(std::string filePath, size_t ts);
std::string getAudioPath(std::string filePath, size_t ts);

int main(int argc, char* argv[])
{
    CloseWriteWatcher *watch = new CloseWriteWatcher();
    
    watch->setWatchFolder("./tmp");
    watch->setCallback(testFunct);
    
    watch->startWatching(run);
    
    delete watch;
};



void testFunct2(std::string filePath)
{
    Demuxer* demux = NULL;
    DashVideoSegmenter* vSeg = NULL;
    DashAudioSegmenter* aSeg = NULL;
    DashSegment* vSegment = NULL;
    DashSegment* aSegment = NULL;
    DashSegment* vInitSegment = NULL;
    DashSegment* aInitSegment = NULL;

    AVCCFrame* videoFrame;
    AACFrame* audioFrame;
    Frame* frame;
    int gotFrame = 0;
    int seqNumber = -1;

    seqNumber = getSeqNumberFromPath(filePath);

    demux = new Demuxer();
    vSeg = new DashVideoSegmenter();
    aSeg = new DashAudioSegmenter();
    vSegment = new DashSegment(vSeg->getMaxSegmentLength(), seqNumber);
    vInitSegment = new DashSegment(vSeg->getMaxSegmentLength(), seqNumber);
    aSegment = new DashSegment(aSeg->getMaxSegmentLength(), seqNumber);
    aInitSegment = new DashSegment(aSeg->getMaxSegmentLength(), seqNumber);

    if (!demux || !vSeg || !aSeg || !vSegment || !vInitSegment || !aSegment || !aInitSegment) {
        std::cerr << "Error constructing objects" << std::endl;
        exit(1);
    }
    
    demux->openInput(filePath);
    demux->findStreams();

    if (demux->hasVideo()) {
        if (!vSeg->init(demux->getVideoDuration(), demux->getVideoTimeBase(), 
                        demux->getVideoSampleDuration(), 
                        demux->getWidth(), demux->getHeight(), demux->getFPS())) {
            std::cerr << "Error initializing Video Segmenter" << std::endl;
            exit(1);
        }

        if (!vSeg->generateInit(demux->getVideoExtraData(), demux->getVideoExtraDataLength(), vInitSegment)) {
            std::cerr << "Error constructing video init" << std::endl;
            exit(1);
        }
        
        vInitSegment->writeToDisk(getVideoInitPath(filePath));
    }

    if (demux->hasAudio()) {
        if (!aSeg->init(demux->getAudioDuration(), demux->getAudioTimeBase(), demux->getAudioSampleDuration(), 
                        demux->getAudioChannels(), demux->getAudioSampleRate(), demux->getAudioBitsPerSample())) {
            std::cerr << "Error initializing Audio Segmenter" << std::endl;
            exit(1);
        }

        if (!aSeg->generateInit(demux->getAudioExtraData(), demux->getAudioExtraDataLength(), aInitSegment)) {
            std::cerr << "Error constructing audio DashSegment objects" << std::endl;
            exit(1);
        }
        
        aInitSegment->writeToDisk(getAudioInitPath(filePath));
    }

    while (gotFrame >= 0){

        frame = demux->readFrame(gotFrame);

        if ((videoFrame = dynamic_cast<AVCCFrame*>(frame)) != NULL) {
            if (vSeg->addToSegment(videoFrame, vSegment)) {
                vSegment->writeToDisk(getVideoPath(filePath, vSegment->getTimestamp()));
            }
        }

        if ((audioFrame = dynamic_cast<AACFrame*>(frame)) != NULL) {

            if (aSeg->addToSegment(audioFrame, aSegment)) {
                aSegment->writeToDisk(getAudioPath(filePath, aSegment->getTimestamp()));
            }
        }
    }

    if (demux->hasVideo() && vSeg->finishSegment(vSegment)) {
        vSegment->writeToDisk(getVideoPath(filePath, vSegment->getTimestamp()));
    }

    if (demux->hasAudio() && aSeg->finishSegment(aSegment)) {
        aSegment->writeToDisk(getAudioPath(filePath, aSegment->getTimestamp()));
    }

    delete demux;
    delete vSeg;
    delete aSeg;
    delete vSegment;
    delete aSegment;
    delete vInitSegment;
    delete aInitSegment;

}

int getSeqNumberFromPath(std::string filePath)
{
    int seqNumber = -1;

    size_t b = filePath.find_last_of("_");
    size_t e = filePath.find_last_of(".");

    std::string stringSequenceNumber = filePath.substr(b+1,e-b-1);
    seqNumber = stoi(stringSequenceNumber);

    return seqNumber;
}

std::string getVideoInitPath(std::string filePath)
{
    std::string path;
    size_t e = filePath.find_last_of("_");
    path = filePath.substr(0,e) + "_init.m4v";
    e = path.find_last_of("/");
    if (e != std::string::npos){
        path = destinationPath + path.substr(e);
    }

    return path;
}

std::string getAudioInitPath(std::string filePath)
{
    std::string path;
    size_t e = filePath.find_last_of("_");
    path = filePath.substr(0,e) + "_init.m4a";
    e = path.find_last_of("/");
    if (e != std::string::npos){
        path = destinationPath + path.substr(e);
    }

    return path;
}

std::string getVideoPath(std::string filePath, size_t ts)
{
    std::string path;
    std::string timestamp = std::to_string(ts);
    size_t e = filePath.find_last_of("_");
    path = filePath.substr(0,e) + "_" + timestamp + ".m4v";
    e = path.find_last_of("/");
     if (e != std::string::npos){
        path = destinationPath + path.substr(e);
    }

    return path;
}

std::string getAudioPath(std::string filePath, size_t ts)
{
    std::string path;
    std::string timestamp = std::to_string(ts);
    size_t e = filePath.find_last_of("_");
    path = filePath.substr(0,e) + "_" + timestamp + ".m4a";
    e = path.find_last_of("/");
    if (e != std::string::npos){
        path = destinationPath + path.substr(e);
    }

    return path;
}
