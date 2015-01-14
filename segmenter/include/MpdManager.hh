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
#include "tinyxml2.h"

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

    bool writeSkeleton(const char* fileName);
    bool updateMpd(const char* fileName);

private:
    Mpd* mpd;
};

class Mpd
{
public:
    Mpd();
    virtual ~Mpd();

    void writeToDisk(const char* fileName);

private:
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
    AdaptationSet();
    virtual ~AdaptationSet();
    virtual void toMpd(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement*& adaptSet);

protected:
    bool segmentAlignment;
    int startWithSAP;
    bool subsegmentAlignment;
    int subsegmentStartsWithSAP;
    std::string mimeType;
    int timescale;
    std::string segTemplate;
    std::string initTemplate;
    int segmentDuration;
    std::deque<int> timestamps;

};

class VideoAdaptationSet : public AdaptationSet 
{
public:
    VideoAdaptationSet();
    virtual ~VideoAdaptationSet();
    void toMpd(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement*& adaptSet);

private:
    std::map<std::string, VideoRepresentation*> representations;
    
    int maxWidth;
    int maxHeight;
    std::string par;
    int frameRate;
};

class AudioAdaptationSet : public AdaptationSet 
{
public:
    AudioAdaptationSet();
    virtual ~AudioAdaptationSet();
    void toMpd(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement*& adaptSet);

private:
    std::map<std::string, AudioRepresentation*> representations;

    std::string lang;
    int audioSamplingRate;
    std::string roleSchemeIdUri;
    int roleValue;
};

class VideoRepresentation
{
public:
    VideoRepresentation();
    virtual ~VideoRepresentation();

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
    AudioRepresentation();
    virtual ~AudioRepresentation();

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
