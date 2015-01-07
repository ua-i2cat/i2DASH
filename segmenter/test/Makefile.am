AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = -std=c++11 -Wall -D__STDC_CONSTANT_MACROS -I$(top_srcdir)/segmenter/include -I$(top_srcdir)/libi2dash/include
#bin_PROGRAMS = demuxTest dashAudioSegmenterTest dashVideoSegmenterTest segmenterTestsDataGenerator
bin_PROGRAMS = dashAudioSegmenterTest
demuxTest_SOURCES = ../../libi2dash/src/i2libdash.c ../../libi2dash/src/i2libisoff.c ../../libi2dash/src/h264_stream.c ../src/Demuxer.cpp ../src/Frame.cpp DemuxerTest.cpp
dashAudioSegmenterTest_SOURCES = ../../libi2dash/src/i2libdash.c ../../libi2dash/src/i2libisoff.c ../../libi2dash/src/h264_stream.c ../src/DashAudioSegmenter.cpp ../src/DashSegment.cpp ../src/Frame.cpp DashAudioSegmenterTest.cpp
dashVideoSegmenterTest_SOURCES = ../../libi2dash/src/i2libdash.c ../../libi2dash/src/i2libisoff.c ../../libi2dash/src/h264_stream.c ../src/DashVideoSegmenter.cpp ../src/DashSegment.cpp ../src/Frame.cpp DashVideoSegmenterTest.cpp
segmenterTestsDataGenerator_SOURCES = ../../libi2dash/src/i2libdash.c ../../libi2dash/src/i2libisoff.c ../../libi2dash/src/h264_stream.c ../src/DashAudioSegmenter.cpp ../src/DashVideoSegmenter.cpp ../src/DashSegment.cpp ../src/Frame.cpp ../src/Demuxer.cpp segmenterTestsDataGenerator.cpp

