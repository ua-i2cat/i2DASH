/*
 *  AVCCFrame - Video frame for H264 AVCC format
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
 *  Authors: David Cassany <david.cassany@i2cat.net> 
 *          
 */

#ifndef _FRAME_HH
#define _FRAME_HH

#include <cstddef>

class Frame {
    
public:
    /**
    * Class constructor
    */
    Frame();
    
    /**
    * Class destructor
    */
    virtual ~Frame();
    
    /**
    * @return Pointer to data buffer
    */ 
    unsigned char* getDataBuffer() {return frameBuff;};

    /**
    * @return Data length in bytes
    */ 
    size_t getDataLength() {return frameLen;};

    /**
    * Sets data buffer adress and length
    * @param buff Pointer to data buffer
    * @param length Data length
    */ 
    void setDataBuffer(unsigned char *buff, size_t length);
    
    /**
    * Set presentation time
    * @param pTime Frame presentation time in origin stream time base units
    * @see Demuxer
    */ 
    void setPresentationTime(size_t pTime);

    /**
    * Set decode time
    * @param pTime Frame decode time in origin stream time base units
    * @see Demuxer
    */ 
    void setDecodeTime(size_t dTime);

    /**
    * Set frame duration
    * @param pTime Frame duration in origin stream time base units
    * @see Demuxer
    */ 
    void setDuration(size_t dur);

    /**
    * @return presentationTime Frame presentation time in origin stream time base units
    * @see Demuxer
    */ 
    size_t getPresentationTime() {return presentationTime;};

    /**
    * @return decodeTime Frame decode time in origin stream time base units
    * @see Demuxer
    */ 
    size_t getDecodeTime() {return decodeTime;};
    
    /**
    * @return duration Frame duration in origin stream time base units
    * @see Demuxer
    */ 
    size_t getDuration() {return duration;};
    
    virtual void clearFrame() = 0;
    
protected:
    unsigned char *frameBuff;
    size_t frameLen;
    size_t presentationTime;
    size_t decodeTime;
    size_t duration;
};

class AVCCFrame : public Frame {
    
public:

    /**
    * Class constructor
    */
    AVCCFrame();
    
    /**
    * Class destructor
    */
    ~AVCCFrame();
    
    /**
    * Set video size
    * @param width Frame width in pixels
    * @param height Frame height in pixels
    */ 
    void setVideoSize(int width, int height);
    
    /**
    * @return true if intra frame or false if not
    */ 
    bool isIntra() {return intra;};

    /**
    * Mark intra frame
    * @params isIntra True if intra frame or false if not
    */ 
    void setIntra(bool isIntra);
    
    /**
    * @return Frame width in pixels
    */ 
    int getWidth() {return width;};

    /**
    * @return Frame height in pixels
    */ 
    int getHeight() {return height;};
    
    /**
    * Reset frame values
    */ 
    void clearFrame();
    
private:
    int width, height;
    bool intra;
};

class AACFrame : public Frame {
    
public:
    /**
    * Class constructor
    */
    AACFrame();
    
    /**
    * Class destructor
    */
    ~AACFrame();
    
    /**
    * @return Sample rate in Hz
    */ 
    int getSampleRate() {return sampleRate;};
    
    /**
    * Set sample rate
    * @param sRate Sample rate in Hz
    */ 
    void setSampleRate(int sRate);
    
    /**
    * Reset frame values
    */ 
    void clearFrame();
    
private:
    int sampleRate;
};

#endif