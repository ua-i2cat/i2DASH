#define MPD_SKELETON "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<MPD xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
     xmlns=\"urn:mpeg:dash:schema:mpd:2011\"\
     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\
     xsi:schemaLocation=\"urn:mpeg:DASH:schema:MPD:2011 http://standards.iso.org/ittf/PubliclyAvailableStandards/MPEG-DASH_schema_files/DASH-MPD.xsd\"\
     profiles=\"urn:mpeg:dash:profile:isoff-live:2011\"\
     type=\"dynamic\"\
     minimumUpdatePeriod=\"PT2.0S\"\
     availabilityStartTime=\"2014-10-29T03:07:39\"\
     timeShiftBufferDepth=\"PT16.0S\"\
     suggestedPresentationDelay=\"PT3.0S\"\
     minBufferTime=\"PT6.0S\">\
<ProgramInformation>\
    <Title>oh_happy_day.smil</Title>\
</ProgramInformation>\
<Location>http://193.104.51.172/live/_definst_/smil:oh_happy_day.smil/manifest.mpd</Location>\
<Period id=\"0\" start=\"PT0.0S\">\
    <AdaptationSet id=\"0\" mimeType=\"video/mp4\" maxWidth=\"1920\" maxHeight=\"1080\" par=\"16:9\" frameRate=\"25\" segmentAlignment=\"true\" startWithSAP=\"1\" subsegmentAlignment=\"true\" subsegmentStartsWithSAP=\"1\">\
        <SegmentTemplate timescale=\"90000\" media=\"chunk_ctvideo_cfm4s_rid$RepresentationID$_cs$Time$_mpd.m4s\" initialization=\"chunk_ctvideo_cfm4s_rid$RepresentationID$_cinit_mpd.m4s\">\
            <SegmentTimeline>\
                <S t=\"3327379380\" d=\"180000\"/>\
                <S d=\"180000\"/>\
                <S d=\"180000\"/>\
                <S d=\"180000\"/>\
                <S d=\"180000\"/>\
                <S d=\"180000\"/>\
                <S d=\"180000\"/>\
                <S d=\"180000\"/>\
            </SegmentTimeline>\
        </SegmentTemplate>\
        <Representation id=\"p0a0r0\" codecs=\"avc1.640028\" width=\"1024\" height=\"576\" sar=\"1:1\" bandwidth=\"3500000\" />\
        <Representation id=\"p0a0r1\" codecs=\"avc1.640028\" width=\"1280\" height=\"720\" sar=\"1:1\" bandwidth=\"5000000\" />\
        <Representation id=\"p0a0r2\" codecs=\"avc1.640028\" width=\"1920\" height=\"1080\" sar=\"1:1\" bandwidth=\"7000000\" />\
    </AdaptationSet>\
    <AdaptationSet id=\"1\" mimeType=\"audio/mp4\" lang=\"eng\" segmentAlignment=\"true\" startWithSAP=\"1\" subsegmentAlignment=\"true\" subsegmentStartsWithSAP=\"1\">\
        <Role schemeIdUri=\"urn:mpeg:dash:role:2011\" value=\"main\"/>\
        <SegmentTemplate timescale=\"48000\" media=\"chunk_ctaudio_cfm4s_rid$RepresentationID$_cs$Time$_mpd.m4s\" initialization=\"chunk_ctaudio_cfm4s_rid$RepresentationID$_cinit_mpd.m4s\">\
            <SegmentTimeline>\
                <S t=\"1774570032\" d=\"96000\"/>\
                <S d=\"96000\"/>\
                <S d=\"96000\"/>\
                <S d=\"84480\"/>\
                <S d=\"107520\"/>\
                <S d=\"96000\"/>\
                <S d=\"96000\"/>\
                <S d=\"96000\"/>\
            </SegmentTimeline>\
        </SegmentTemplate>\
        <Representation id=\"p0a1r0\" codecs=\"mp4a.40.2\" audioSamplingRate=\"48000\" bandwidth=\"189000\">\
        <AudioChannelConfiguration schemeIdUri=\"urn:mpeg:dash:23003:3:audio_channel_configuration:2011\" value=\"2\"/>\
        </Representation>\
    </AdaptationSet>\
</Period>\
</MPD>"
