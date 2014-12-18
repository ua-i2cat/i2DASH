/*
 *  Demuxer.hh - Demuxer class
 *  Copyright (C) 2014  Fundació i2CAT, Internet i Innovació digital a Catalunya
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
 *            
 *            
 */

#ifndef _DEMUXER_HH
#define _DEMUXER_HH
extern "C" {
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
}

#include <string>
#include "Frame.hh"

#define N_OF_NAL_SIZE_BYTES_MINUS_ONE_POSITION 4
#define N_OF_NAL_SIZE_BYTES_MINUS_ONE_MASK 0x03
#define NAL_TYPE_MASK 0x1F
#define IDR_NAL_TYPE 5
#define SEI_NAL_TYPE 6

/*! It is a media demuxer based on libavformat. It is used to read a file, to demux it extracting the different audio and video streams and to
    read streams frame by frame. It is constrained to MP4 container, H264 video codec and AAC audio codec. It is also limited to one video stream
    and one audio stream per file (it choose them automatically if the file contains more than one stream of the same type) */ 

class Demuxer {
public:

    /**
    * Class constructor
    * @param vTime Video timestamp initial value
    * @param aTime Audio timestamp initial value
    */
    Demuxer(size_t vTime = 0, size_t aTime = 0);

    /**
    * Class destructor
    */
    ~Demuxer();
    
    /**
    * Open input file
    * @param filename File name
    * @return true if succeeded and false if not
    */
    bool openInput(std::string filename);

    /**
    * Close input file and internal structures related to it
    */
    void closeInput();

    /**
    * Dump file format information to stdout
    */
    void dumpFormat();

    /**
    * Search audio and video streams in the file and configures the demuxer to start reading.
    * It must be done before executing readFrame()
    * @return true if succeeded and false if not
    */
    bool findStreams();

    /**
    * It reads a frame from the file
    * @param gotFrame indicates if we have read a complete frame
    * @return Pointer to the read frame if succeeded and NULL if not
    * see @Frame
    */
    Frame* const readFrame(int& gotFrame);
    
    float getFPS();
    size_t getWidth(){return videoStream->codec->width;};
    size_t getHeight(){return videoStream->codec->height;};
    size_t getVideoBitRate() {return videoStream->codec->bit_rate;};

    /**
    * @return Video time base in ticks
    */
    size_t getVideoTimeBase();

    /**
    * @return Video duration in time base units
    */
    size_t getVideoDuration();

    /**
    * @return Estimated video sample duration in time base units
    */
    size_t getVideoSampleDuration();
    
    size_t getAudioSampleRate() {return audioStream->codec->sample_rate;};
    size_t getAudioChannels() {return audioStream->codec->channels;};
    size_t getAudioBitsPerSample();
    size_t getAudioBitRate() {return audioStream->codec->bit_rate;};

    /**
    * @return audio time base in ticks
    */
    size_t getAudioTimeBase();

    /**
    * @return audio duration in time base units
    */
    size_t getAudioDuration();

    /**
    * @return Estimated audio sample duration in time base units
    */ 
    size_t getAudioSampleDuration();
    
    /**
    * @return True if file has at least one video stream
    */ 
    bool hasVideo();

    /**
    * @return True if file has at least one audio stream
    */ 
    bool hasAudio();

    /**
    * Get video metadata from the media container
    * @return Pointer to data
    */ 
    unsigned char* getVideoExtraData();

    /**
    * Get video metadata size
    * @return Video metadata size in bytes
    */ 
    size_t getVideoExtraDataLength();

    /**
    * Get audio metadata from the media container
    * @return Pointer to data
    */ 
    unsigned char* getAudioExtraData();

    /**
    * Get audio metadata size
    * @return Audio metadata size in bytes
    */ 
    size_t getAudioExtraDataLength();
        
private:
    bool sourceExists(std::string filename);
    bool findVideoStream();
    bool findAudioStream();
    bool validVideoCodec();
    bool validAudioCodec();
    bool isIntra(unsigned char* data);
    size_t getNalSizeBytes(unsigned char* metadata);
          
private:
      
    AVFormatContext *fmtCtx;
    AVPacket pkt;
    AVStream *audioStream, *videoStream;
    
    int videoStreamIdx, audioStreamIdx;
    int framesCounter;
    bool isOpen;
       
    size_t vStartTime; //Video TimeBase Ticks
    size_t aStartTime; //Audio TimeBase Ticks
    
    AVCCFrame* const videoFrame;
    AACFrame* const audioFrame;

    size_t nalSizeBytes;
};

#endif
