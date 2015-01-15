ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = dashSegmenter dashLiveSegmenter
bin_PROGRAMS += demuxTest dashAudioSegmenterTest dashVideoSegmenterTest segmenterTestsDataGenerator 
 
dashSegmenter_SOURCES = ../libi2dash/src/i2libdash.c ../libi2dash/src/i2libisoff.c ../libi2dash/src/h264_stream.c src/DashAudioSegmenter.cpp src/DashSegment.cpp src/DashSegmenter.cpp src/DashVideoSegmenter.cpp src/Demuxer.cpp src/Frame.cpp
dashSegmenter_CPPFLAGS = -std=c++11 -D__STDC_CONSTANT_MACROS -Wall -Iinclude -I../libi2dash/include

demuxTest_SOURCES = ../libi2dash/src/i2libdash.c ../libi2dash/src/i2libisoff.c ../libi2dash/src/h264_stream.c src/Demuxer.cpp src/Frame.cpp test/DemuxerTest.cpp
demuxTest_CPPFLAGS = -std=c++11 -Wall -D__STDC_CONSTANT_MACROS -Iinclude -I../libi2dash/include

dashAudioSegmenterTest_SOURCES = ../libi2dash/src/i2libdash.c ../libi2dash/src/i2libisoff.c ../libi2dash/src/h264_stream.c src/DashAudioSegmenter.cpp src/DashSegment.cpp src/Frame.cpp test/DashAudioSegmenterTest.cpp
dashAudioSegmenterTest_CPPFLAGS = -std=c++11 -Wall -Iinclude -I../libi2dash/include

dashVideoSegmenterTest_SOURCES = ../libi2dash/src/i2libdash.c ../libi2dash/src/i2libisoff.c ../libi2dash/src/h264_stream.c src/DashVideoSegmenter.cpp src/DashSegment.cpp src/Frame.cpp test/DashVideoSegmenterTest.cpp
dashVideoSegmenterTest_CPPFLAGS = -std=c++11 -Wall -Iinclude -I../libi2dash/include

mpdManagerTest_SOURCES = src/MpdManager.cpp test/MpdManagerTest.cpp
mpdManagerTest_CPPFLAGS = -std=c++11 -Wall -Iinclude

segmenterTestsDataGenerator_SOURCES = ../libi2dash/src/i2libdash.c ../libi2dash/src/i2libisoff.c ../libi2dash/src/h264_stream.c src/DashAudioSegmenter.cpp src/DashVideoSegmenter.cpp src/DashSegment.cpp src/Frame.cpp src/Demuxer.cpp test/segmenterTestsDataGenerator.cpp
segmenterTestsDataGenerator_CPPFLAGS = -std=c++11 -Wall -D__STDC_CONSTANT_MACROS -Iinclude -I../libi2dash/include

dashLiveSegmenter_SOURCES = ../libi2dash/src/i2libdash.c ../libi2dash/src/i2libisoff.c ../libi2dash/src/h264_stream.c src/DashAudioSegmenter.cpp src/DashSegment.cpp src/CloseWriteWatcher.cpp src/DashLiveSegmenter.cpp src/DashVideoSegmenter.cpp src/Demuxer.cpp src/MP4ToDashConverter.cpp src/Frame.cpp src/MpdManager.cpp
dashLiveSegmenter_CPPFLAGS = -std=c++11 -Wall -D__STDC_CONSTANT_MACROS -Iinclude -I../libi2dash/include
