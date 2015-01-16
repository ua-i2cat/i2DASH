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
 *           Marc Palau <marc.palau@i2cat.net>
 */

#include <fstream>
#include <tinyxml2.h>
#include <iostream>

#include "MpdManager.hh"

MpdManager::MpdManager()
{
}

MpdManager::~MpdManager()
{
}

void MpdManager::setMinimumUpdatePeriod(int seconds)
{
    minimumUpdatePeriod = "PT" + std::to_string(seconds) + ".0S";
}

void MpdManager::setMinBufferTime(int seconds)
{
    minBufferTime = "PT" + std::to_string(seconds) + ".0S";
}

void MpdManager::setTimeShiftBufferDepth(int seconds)
{
    timeShiftBufferDepth = "PT" + std::to_string(seconds) + ".0S";
}

void MpdManager::writeToDisk(const char* fileName)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root;
    tinyxml2::XMLElement* period;
    tinyxml2::XMLElement* el;
    tinyxml2::XMLElement* title;
    tinyxml2::XMLText* text;

    //TODO: set hardcodedd values to defines
    root = doc.NewElement("MPD");
    root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root->SetAttribute("xmlns", "urn:mpeg:dash:schema:mpd:2011");
    root->SetAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    root->SetAttribute("xsi:schemaLocation", "urn:mpeg:DASH:schema:MPD:2011 http://standards.iso.org/ittf/PubliclyAvailableStandards/MPEG-DASH_schema_files/DASH-MPD.xsd");
    root->SetAttribute("profiles", "urn:mpeg:dash:profile:isoff-live:2011");
    root->SetAttribute("type", "dynamic");
    root->SetAttribute("minimumUpdatePeriod", minimumUpdatePeriod.c_str());
    root->SetAttribute("availabilityStartTime", "2014-10-29T03:07:39");
    root->SetAttribute("timeShiftBufferDepth", timeShiftBufferDepth.c_str());
    root->SetAttribute("minBufferTime", minBufferTime.c_str());
    doc.InsertFirstChild(root);

    el = doc.NewElement("ProgramInformation");
    title = doc.NewElement("Title");
    text = doc.NewText("Demo DASH");
    title->InsertFirstChild(text);
    el->InsertFirstChild(title);
    root->InsertEndChild(el);

    el = doc.NewElement("Location");
    text = doc.NewText(location.c_str());
    el->InsertFirstChild(text);
    root->InsertEndChild(el);

    period = doc.NewElement("Period");
    period->SetAttribute("id", 0);
    period->SetAttribute("start", "PT0.0S");

    for (auto ad : adaptationSets) {
        el = doc.NewElement("AdaptationSet");
        el->SetAttribute("id", ad.first.c_str());
        ad.second->toMpd(doc, el);
        period->InsertEndChild(el);
    }

    root->InsertEndChild(period);
    doc.SaveFile(fileName);
}

void MpdManager::setLocation(std::string loc)
{
    location = loc;
}

bool MpdManager::updateAdaptationSetTimestamp(std::string id, int ts, int duration)
{
    AdaptationSet* adSet;

    adSet = getAdaptationSet(id);

    if (!adSet) {
        return false;
    }

    adSet->updateTimestamp(ts, duration);
    return true;
}

void MpdManager::updateVideoAdaptationSet(std::string id, int timescale, std::string segmentTempl, std::string initTempl)
{
    AdaptationSet* adSet;

    adSet = getAdaptationSet(id);

    if (!adSet) {
        adSet = createVideoAdaptationSet(timescale, segmentTempl, initTempl);
        addAdaptationSet(id, adSet);
    }

    adSet->update(timescale, segmentTempl, initTempl);
}

void MpdManager::updateAudioAdaptationSet(std::string id, int timescale, std::string segmentTempl, std::string initTempl)
{
    AdaptationSet* adSet;

    adSet = getAdaptationSet(id);

    if (!adSet) {
        adSet = createAudioAdaptationSet(timescale, segmentTempl, initTempl);
        addAdaptationSet(id, adSet);
    }

    adSet->update(timescale, segmentTempl, initTempl);
}

void MpdManager::updateVideoRepresentation(std::string adSetId, std::string reprId, std::string codec, int width, int height, int bandwidth, int fps)
{
    AdaptationSet* adSet;

    adSet = getAdaptationSet(adSetId);

    if (!adSet) {
        std::cerr << "Error updating video representation. Adaptation set does not exist." << std::endl;
        return;
    }

    adSet->updateVideoRepresentation(reprId, codec, width, height, bandwidth, fps);
}

void MpdManager::updateAudioRepresentation(std::string adSetId, std::string reprId, std::string codec, int sampleRate, int bandwidth, int channels)
{
    AdaptationSet* adSet;

    adSet = getAdaptationSet(adSetId);

    if (!adSet) {
        std::cerr << "Error updating audio representation. Adaptation set does not exist." << std::endl;
        return;
    }

    adSet->updateAudioRepresentation(reprId, codec, sampleRate, bandwidth, channels);
}

//TODO:: check if necessary
AdaptationSet* MpdManager::createVideoAdaptationSet(int timescale, std::string segmentTempl, std::string initTempl)
{
    return new VideoAdaptationSet(timescale, segmentTempl, initTempl);
}

//TODO:: check if necessary
AdaptationSet* MpdManager::createAudioAdaptationSet(int timescale, std::string segmentTempl, std::string initTempl)
{
    return new AudioAdaptationSet(timescale, segmentTempl, initTempl);
}

bool MpdManager::addAdaptationSet(std::string id, AdaptationSet* adaptationSet)
{
    if (adaptationSets.count(id) > 0) {
        return false;
    }

    adaptationSets[id] = adaptationSet;
    return true;
}


AdaptationSet* MpdManager::getAdaptationSet(std::string id)
{
    return adaptationSets.count(id) <= 0 ? NULL : adaptationSets[id];
}

//TODO:: hardcodeds to defines
AdaptationSet::AdaptationSet(int segTimescale, std::string segTempl, std::string initTempl)
{
    timescale = segTimescale;
    segTemplate = segTempl;
    initTemplate = initTempl;
    segmentAlignment = true;
    startWithSAP = 1;
    subsegmentAlignment = true;
    subsegmentStartsWithSAP = 1;
}

AdaptationSet::~AdaptationSet()
{ 
}

void AdaptationSet::updateTimestamp(int ts, int duration)
{
    for (auto listTs : timestamps){
        if (listTs.first == ts) {
            return;
        }
    }

    if (timestamps.size() >= MAX_SEGMENTS_IN_MPD) {
        timestamps.pop_front();
    }

    timestamps.push_back(std::pair<int,int>(ts, duration));

}

void AdaptationSet::update(int segTimescale, std::string segTempl, std::string initTempl)
{
    timescale = segTimescale;
    segTemplate = segTempl;
    initTemplate = initTempl;
}

//TODO:: hardcodeds to defines
VideoAdaptationSet::VideoAdaptationSet(int segTimescale, std::string segTempl, std::string initTempl)
: AdaptationSet(segTimescale, segTempl, initTempl)
{
    mimeType = "video/mp4";
    frameRate = 0;
    maxWidth = 0;
    maxHeight = 0;
}

VideoAdaptationSet::~VideoAdaptationSet()
{ 
}

VideoRepresentation* VideoAdaptationSet::getRepresentation(std::string id)
{
    return representations.count(id) <= 0 ? NULL : representations[id];
}


void VideoAdaptationSet::updateVideoRepresentation(std::string id, std::string codec, int width, int height, int bandwidth, int fps)
{
    VideoRepresentation* vRepr;

    vRepr = getRepresentation(id);

    if (!vRepr) {
        vRepr = createRepresentation(codec, width, height, bandwidth, fps);
        addRepresentation(id, vRepr);
    } else {
        vRepr->update(codec, width, height, bandwidth);
    }

    if (maxWidth < width) {
        maxWidth = width;
    }

    if (maxHeight < height) {
        maxHeight = height;
    }

    frameRate = fps;
}

//TODO: check if necessary
VideoRepresentation* VideoAdaptationSet::createRepresentation(std::string codec, int width, int height, int bandwidth, int fps)
{
    return new VideoRepresentation(codec, width, height, bandwidth); 
}

bool VideoAdaptationSet::addRepresentation(std::string id, VideoRepresentation* repr)
{
    if (representations.count(id) > 0) {
        return false;
    }

    representations[id] = repr;
    return true;
}

void VideoAdaptationSet::toMpd(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement*& adaptSet)
{
    tinyxml2::XMLElement* segmentTemplate;
    tinyxml2::XMLElement* segmentTimeline;
    tinyxml2::XMLElement* s;
    tinyxml2::XMLElement* repr;

    adaptSet->SetAttribute("mimeType", mimeType.c_str());
    adaptSet->SetAttribute("maxWidth", maxWidth);
    adaptSet->SetAttribute("maxHeight", maxHeight);
    adaptSet->SetAttribute("frameRate", frameRate);
    adaptSet->SetAttribute("segmentAlignment", segmentAlignment);
    adaptSet->SetAttribute("startWithSAP", startWithSAP);
    adaptSet->SetAttribute("subsegmentAlignment", subsegmentAlignment);
    adaptSet->SetAttribute("subsegmentStartsWithSAP", subsegmentStartsWithSAP);

    segmentTemplate = doc.NewElement("SegmentTemplate");
    segmentTemplate->SetAttribute("timescale", timescale);
    segmentTemplate->SetAttribute("media", segTemplate.c_str());
    segmentTemplate->SetAttribute("initialization", initTemplate.c_str());

    segmentTimeline = doc.NewElement("SegmentTimeline");

    for (auto ts : timestamps) {
        s = doc.NewElement("S");
        s->SetAttribute("t", ts.first);
        s->SetAttribute("d", ts.second);
        segmentTimeline->InsertEndChild(s);
    }

    segmentTemplate->InsertEndChild(segmentTimeline);
    adaptSet->InsertEndChild(segmentTemplate);

    for (auto r : representations) {
        repr = doc.NewElement("Representation");
        repr->SetAttribute("id", r.first.c_str());
        repr->SetAttribute("codecs", r.second->getCodec().c_str());
        repr->SetAttribute("width", r.second->getWidth());
        repr->SetAttribute("height", r.second->getHeight());
        repr->SetAttribute("sar", r.second->getSAR().c_str());
        repr->SetAttribute("bandwidth", r.second->getBandwidth());
        adaptSet->InsertEndChild(repr);
    }
}

//TODO: hardcodeds to defines
AudioAdaptationSet::AudioAdaptationSet(int segTimescale, std::string segTempl, std::string initTempl)
: AdaptationSet(segTimescale, segTempl, initTempl)
{
    mimeType = "audio/mp4";
    lang = "eng";
    roleSchemeIdUri = "urn:mpeg:dash:role:2011";
    roleValue = "main";
}

AudioAdaptationSet::~AudioAdaptationSet()
{ 
}

AudioRepresentation* AudioAdaptationSet::getRepresentation(std::string id)
{
    return representations.count(id) <= 0 ? NULL : representations[id];
}

void AudioAdaptationSet::updateAudioRepresentation(std::string id, std::string codec, int sampleRate, int bandwidth, int channels)
{
    AudioRepresentation* repr;

    repr = getRepresentation(id);

    if (!repr) {
        repr = createRepresentation(codec, sampleRate, bandwidth, channels);
        addRepresentation(id, repr);
        return;
    }

    repr->update(codec, sampleRate, bandwidth, channels);
}

//TODO:: check if necessary
AudioRepresentation* AudioAdaptationSet::createRepresentation(std::string codec, int sampleRate, int bandwidth, int channels)
{
    return new AudioRepresentation(codec, sampleRate, bandwidth, channels);
}


bool AudioAdaptationSet::addRepresentation(std::string id, AudioRepresentation* repr)
{
    if (representations.count(id) > 0) {
        return false;
    }

    representations[id] = repr;
    return true;
}

void AudioAdaptationSet::toMpd(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement*& adaptSet)
{
    tinyxml2::XMLElement* segmentTemplate;
    tinyxml2::XMLElement* segmentTimeline;
    tinyxml2::XMLElement* role;
    tinyxml2::XMLElement* audioChannelConfiguration;
    tinyxml2::XMLElement* s;
    tinyxml2::XMLElement* repr;

    adaptSet->SetAttribute("mimeType", mimeType.c_str());
    adaptSet->SetAttribute("lang", lang.c_str());
    adaptSet->SetAttribute("segmentAlignment", segmentAlignment);
    adaptSet->SetAttribute("startWithSAP", startWithSAP);
    adaptSet->SetAttribute("subsegmentAlignment", subsegmentAlignment);
    adaptSet->SetAttribute("subsegmentStartsWithSAP", subsegmentStartsWithSAP);

    role = doc.NewElement("Role");
    role->SetAttribute("schemeIdUri", roleSchemeIdUri.c_str());
    role->SetAttribute("value", roleValue.c_str());
    adaptSet->InsertEndChild(role);

    segmentTemplate = doc.NewElement("SegmentTemplate");
    segmentTemplate->SetAttribute("timescale", timescale);
    segmentTemplate->SetAttribute("media", segTemplate.c_str());
    segmentTemplate->SetAttribute("initialization", initTemplate.c_str());

    segmentTimeline = doc.NewElement("SegmentTimeline");

    for (auto ts : timestamps) {
        s = doc.NewElement("S");
        s->SetAttribute("t", ts.first);
        s->SetAttribute("d", ts.second);
        segmentTimeline->InsertEndChild(s);
    }

    segmentTemplate->InsertEndChild(segmentTimeline);
    adaptSet->InsertEndChild(segmentTemplate);

    for (auto r : representations) {
        repr = doc.NewElement("Representation");
        repr->SetAttribute("id", r.first.c_str());
        repr->SetAttribute("codecs", r.second->getCodec().c_str());
        repr->SetAttribute("audioSamplingRate", r.second->getSampleRate());
        repr->SetAttribute("bandwidth", r.second->getBandwidth());

        audioChannelConfiguration = doc.NewElement("AudioChannelConfiguration");
        audioChannelConfiguration->SetAttribute("schemeIdUri", r.second->getAudioChannelConfigSchemeIdUri().c_str());
        audioChannelConfiguration->SetAttribute("value", r.second->getAudioChannelConfigValue());
        repr->InsertEndChild(audioChannelConfiguration);

        adaptSet->InsertEndChild(repr);
    }
}

//TODO: harcdoded to define
VideoRepresentation::VideoRepresentation(std::string vCodec, int vWidth, int vHeight, int vBandwidth)
{
    codec = vCodec;
    width = vWidth;
    height = vHeight;
    bandwidth = vBandwidth;
    sar = "1:1";
}

VideoRepresentation::~VideoRepresentation()
{
}

void VideoRepresentation::update(std::string vCodec, int vWidth, int vHeight, int vBandwidth)
{
    codec = vCodec;
    width = vWidth;
    height = vHeight;
    bandwidth = vBandwidth;
}

//TODO: harcdoded to define
AudioRepresentation::AudioRepresentation(std::string aCodec, int aSampleRate, int aBandwidth, int channels)
{
    codec = aCodec;
    sampleRate = aSampleRate;
    bandwidth = aBandwidth;
    audioChannelConfigSchemeIdUri = "urn:mpeg:dash:23003:3:audio_channel_configuration:2011";
    audioChannelConfigValue = channels;
}

AudioRepresentation::~AudioRepresentation()
{
}

void AudioRepresentation::update(std::string aCodec, int aSampleRate, int aBandwidth, int channels)
{
    codec = aCodec;
    sampleRate = aSampleRate;
    bandwidth = aBandwidth;
    audioChannelConfigValue = channels;
}

