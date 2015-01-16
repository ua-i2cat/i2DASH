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

#ifndef _MPD_MANAGER_HH_
#define _MPD_MANAGER_HH_

#include <map>
#include <deque>
#include <tinyxml2.h>

#define MAX_SEGMENTS_IN_MPD 6

class Mpd;
class AdaptationSet;
class VideoAdaptationSet;
class AudioAdaptationSet;
class VideoRepresentation;
class AudioRepresentation;

class MpdManager
{
public:
    MpdManager();
    virtual ~MpdManager();

    Mpd* getMpd(){return mpd;};

private:
    Mpd* mpd;
};

class Mpd
{
public:
    Mpd();
    virtual ~Mpd();

    void writeToDisk(const char* fileName);
    void setLocation(std::string loc);
    void setMinimumUpdatePeriod(int seconds);
    void setMinBufferTime(int seconds);
    void setSuggestedPresentationDelay(int seconds);
    void setTimeShiftBufferDepth(int seconds);
    void updateVideoAdaptationSet(std::string id, int timescale, std::string segmentTempl, std::string initTempl);
    void updateAudioAdaptationSet(std::string id, int timescale, std::string segmentTempl, std::string initTempl);
    bool updateAdaptationSetTimestamp(std::string id, int ts, int duration);
    void updateVideoRepresentation(std::string adSetId, std::string reprId, std::string codec, int width, int height, int bandwidth, int fps);
    void updateAudioRepresentation(std::string adSetId, std::string reprId, std::string codec, int sampleRate, int bandwidth, int channels);

private:
    bool addAdaptationSet(std::string id, AdaptationSet* adaptationSet);
    AdaptationSet* createVideoAdaptationSet(int timescale, std::string segmentTempl, std::string initTempl);
    AdaptationSet* createAudioAdaptationSet(int timescale, std::string segmentTempl, std::string initTempl);
    AdaptationSet* getAdaptationSet(std::string id);

    std::string minimumUpdatePeriod;
    std::string timeShiftBufferDepth;
    std::string suggestedPresentationDelay;
    std::string minBufferTime;
    std::string location;

    std::map<std::string, AdaptationSet*> adaptationSets;
};

class AdaptationSet
{
public:
    AdaptationSet(int segTimescale, std::string segTempl, std::string initTempl);
    virtual ~AdaptationSet();
    virtual void toMpd(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement*& adaptSet) = 0;
    virtual void updateVideoRepresentation(std::string id, std::string codec, int width, int height, int bandwidth, int fps){};
    virtual void updateAudioRepresentation(std::string id, std::string codec, int sampleRate, int bandwidth, int channels){};
    void update(int timescale, std::string segmentTempl, std::string initTempl);
    void updateTimestamp(int ts, int duration);

protected:
    bool segmentAlignment;
    int startWithSAP;
    bool subsegmentAlignment;
    int subsegmentStartsWithSAP;
    std::string mimeType;
    int timescale;
    std::string segTemplate;
    std::string initTemplate;
    std::deque<std::pair<int,int> > timestamps;
};

class VideoAdaptationSet : public AdaptationSet 
{
public:
    VideoAdaptationSet(int segTimescale, std::string segTempl, std::string initTempl);
    virtual ~VideoAdaptationSet();
    void toMpd(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement*& adaptSet);
    void updateVideoRepresentation(std::string id, std::string codec, int width, int height, int bandwidth, int fps);

private:
    VideoRepresentation* getRepresentation(std::string id);
    VideoRepresentation* createRepresentation(std::string codec, int width, int height, int bandwidth, int fps);
    bool addRepresentation(std::string id, VideoRepresentation* repr);
    
    std::map<std::string, VideoRepresentation*> representations;
    int maxWidth;
    int maxHeight;
    int frameRate;
};

class AudioAdaptationSet : public AdaptationSet 
{
public:
    AudioAdaptationSet(int segTimescale, std::string segTempl, std::string initTempl);
    virtual ~AudioAdaptationSet();
    void toMpd(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement*& adaptSet);
    void updateAudioRepresentation(std::string id, std::string codec, int sampleRate, int bandwidth, int channels);

private:
    AudioRepresentation* getRepresentation(std::string id);
    AudioRepresentation* createRepresentation(std::string codec, int sampleRate, int bandwidth, int channels);
    bool addRepresentation(std::string id, AudioRepresentation* repr);
    
    std::map<std::string, AudioRepresentation*> representations;
    std::string lang;
    std::string roleSchemeIdUri;
    std::string roleValue;
};

class VideoRepresentation
{
public:
    VideoRepresentation(std::string vCodec, int vWidth, int vHeight, int vBandwidth);
    virtual ~VideoRepresentation();

    void update(std::string vCodec, int vWidth, int vHeight, int vBandwidth);

    std::string getCodec() {return codec;};
    std::string getSAR() {return sar;};
    int getWidth() {return width;};
    int getHeight() {return height;};
    int getBandwidth() {return bandwidth;};

private:
    std::string codec;
    std::string sar;
    int width;
    int height;
    int bandwidth;
};

class AudioRepresentation
{
public:
    AudioRepresentation(std::string aCodec, int aSampleRate, int aBandwidth, int channels);
    virtual ~AudioRepresentation();

    void update(std::string aCodec, int aSampleRate, int aBandwidth, int channels);

    std::string getCodec() {return codec;};
    int getSampleRate() {return sampleRate;};
    std::string getAudioChannelConfigSchemeIdUri() {return audioChannelConfigSchemeIdUri;};
    int getBandwidth() {return bandwidth;};
    int getAudioChannelConfigValue() {return audioChannelConfigValue;};

private:
    std::string codec;
    int sampleRate;
    int bandwidth;
    std::string audioChannelConfigSchemeIdUri;
    int audioChannelConfigValue;
};

#endif /* _MPD_MANAGER_HH_ */
