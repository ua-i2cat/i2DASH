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

#include <fstream>
#include <tinyxml2.h>

#include "MpdManager.hh"

MpdManager::MpdManager()
{
}

MpdManager::~MpdManager()
{
}

void Mpd::writeToDisk(const char* fileName)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root;
    tinyxml2::XMLElement* period;
    tinyxml2::XMLElement* el;

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
    root->SetAttribute("suggestedPresentationDelay", suggestedPresentationDelay.c_str());
    root->SetAttribute("minBufferTime", minBufferTime.c_str());
    doc.InsertFirstChild(root);

    el = doc.NewElement("ProgramInformation");
    el->SetText("Demo DASH");
    root->InsertEndChild(el);

    el = doc.NewElement("Location");
    el->SetText(location.c_str());
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

    root->InsertFirstChild(period);
    doc.SaveFile("SavedData.xml");
}

void Mpd::setLocation(std::string loc)
{
    location = loc;
}

AdaptationSet* Mpd::createVideoAdaptationSet(int timescale, std::string segmentTempl, std::string initTempl, int segmentDur, int fps)
{
    AdaptationSet* adaptSet = NULL;

    adaptSet = new VideoAdaptationSet(timescale, segmentTempl, initTempl, segmentDur, fps);

    return adaptSet;
}

AdaptationSet* Mpd::createAudioAdaptationSet(int timescale, std::string segmentTempl, std::string initTempl, int segmentDur)
{
    AdaptationSet* adaptSet = NULL;

    adaptSet = new AudioAdaptationSet(timescale, segmentTempl, initTempl, segmentDur);

    return adaptSet;
}

bool Mpd::addAdaptationSet(std::string id, AdaptationSet* adaptationSet)
{
    if (adaptationSets.count(id) > 0) {
        return false;
    }

    adaptationSets[id] = adaptationSet;
    return true;
}


AdaptationSet* Mpd::getAdaptationSet(std::string id)
{
    if (adaptationSets.count(id) <= 0) {
        return NULL;
    }

    return adaptationSets[id];
}

AdaptationSet::AdaptationSet(int segTimescale, std::string segTempl, std::string initTempl, int segDur)
{
    timescale = segTimescale;
    segTemplate = segTempl;
    initTemplate = initTempl;
    segmentDuration = segDur;
    segmentAlignment = true;
    startWithSAP = 1;
    subsegmentAlignment = true;
    subsegmentStartsWithSAP = 1;
}

AdaptationSet::~AdaptationSet()
{ 
}

void AdaptationSet::updateTimestamp(int ts)
{
    if (timestamps.size() >= MAX_SEGMENTS_IN_MPD) {
        timestamps.pop_front();
    }

    timestamps.push_back(ts);
}


VideoAdaptationSet::VideoAdaptationSet(int segTimescale, std::string segTempl, std::string initTempl, int segDur, int fps)
: AdaptationSet(segTimescale, segTempl, initTempl, segDur)
{
    mimeType = "video/mp4";
    frameRate = fps;
    maxWidth = 0;
    maxHeight = 0;
    par = "";
}

VideoAdaptationSet::~VideoAdaptationSet()
{ 
}

bool VideoAdaptationSet::addVideoRepresentation(std::string id, std::string codec, int width, int height, int bandwidth)
{
    VideoRepresentation* vRepr;

    if (representations.count(id) > 0) {
        return false;
    }

    vRepr = new VideoRepresentation(codec, width, height, bandwidth);
    representations[id] = vRepr;

    if (maxWidth < width) {
        maxWidth = width;
    }

    if (maxHeight < height) {
        maxHeight = height;
    }

    //TODO: calculate par
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
    adaptSet->SetAttribute("par", par.c_str());
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
        s->SetAttribute("t", ts);
        s->SetAttribute("d", segmentDuration);
        segmentTimeline->InsertEndChild(s);
    }

    segmentTemplate->InsertEndChild(segmentTimeline);

    for (auto r : representations) {
        repr = doc.NewElement("Representation");
        repr->SetAttribute("id", r.first.c_str());
        repr->SetAttribute("codecs", r.second->getCodec().c_str());
        repr->SetAttribute("width", r.second->getWidth());
        repr->SetAttribute("height", r.second->getHeight());
        repr->SetAttribute("sar", r.second->getSAR().c_str());
        repr->SetAttribute("bandwidth", r.second->getBandwidth());
        segmentTemplate->InsertEndChild(repr);
    }

    adaptSet->InsertEndChild(segmentTemplate);
}

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

AudioAdaptationSet::AudioAdaptationSet(int segTimescale, std::string segTempl, std::string initTempl, int segDur)
: AdaptationSet(segTimescale, segTempl, initTempl, segDur)
{
    mimeType = "audio/mp4";
    lang = "eng";
    roleSchemeIdUri = "urn:mpeg:dash:role:2011";
    roleValue = "main";
}

AudioAdaptationSet::~AudioAdaptationSet()
{ 
}

bool AudioAdaptationSet::addAudioRepresentation(std::string id, std::string codec, int sampleRate, int bandwidth, int channels)
{
    AudioRepresentation* aRepr;

    if (representations.count(id) > 0) {
        return false;
    }

    aRepr = new AudioRepresentation(codec, sampleRate, bandwidth, channels);
    representations[id] = aRepr;
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
        s->SetAttribute("t", ts);
        s->SetAttribute("d", segmentDuration);
        segmentTimeline->InsertEndChild(s);
    }

    segmentTemplate->InsertEndChild(segmentTimeline);

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

        segmentTemplate->InsertEndChild(repr);
    }

    adaptSet->InsertEndChild(segmentTemplate);
}

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
