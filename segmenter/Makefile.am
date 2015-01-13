AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = -std=c++11 -Wall -D__STDC_CONSTANT_MACROS -I$(top_srcdir)/segmenter/include -I$(top_srcdir)/libi2dash/include
bin_PROGRAMS = dashSegmenter
dashSegmenter_SOURCES = ../../libi2dash/src/i2libdash.c ../../libi2dash/src/i2libisoff.c ../../libi2dash/src/h264_stream.c DashAudioSegmenter.cpp DashSegment.cpp DashSegmenter.cpp DashVideoSegmenter.cpp Demuxer.cpp Frame.cpp

