/*
 *  MpdManagerTest.cpp - MpdManager class test
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
 *  Authors:  Xavier Carol <xavier.carol@i2cat.net>
 *
 *
 */
#include <unistd.h>

#include "MpdManager.hh"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/XmlOutputter.h>

class MpdManagerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MpdManagerTest);
    CPPUNIT_TEST(buildFirstMpd);
    CPPUNIT_TEST(updateMpd);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void buildFirstMpd();
    void updateMpd();

protected:
    MpdManager* manager = NULL;
};

void MpdManagerTest::setUp()
{
    manager = new MpdManager();
}

void MpdManagerTest::tearDown()
{
    delete manager;
}

void MpdManagerTest::buildFirstMpd()
{
    const char *TEST_FILE_OK = "/tmp/test.mpd";
    const char *TEST_FILE_KO = "/test.mpd";

    CPPUNIT_ASSERT(manager->writeSkeleton(TEST_FILE_OK));
    CPPUNIT_ASSERT(!manager->writeSkeleton(TEST_FILE_KO));
    CPPUNIT_ASSERT(unlink(TEST_FILE_OK) == 0);
}

void MpdManagerTest::updateMpd()
{
    const char *TEST_FILE_OK = "/tmp/test.mpd";
    const char *TEST_FILE_KO = "/test.mpd";

    CPPUNIT_ASSERT(manager->writeSkeleton(TEST_FILE_OK));
    CPPUNIT_ASSERT(manager->updateMpd(TEST_FILE_OK));
    CPPUNIT_ASSERT(unlink(TEST_FILE_OK) == 0);

    CPPUNIT_ASSERT(!manager->updateMpd(TEST_FILE_KO));
}

CPPUNIT_TEST_SUITE_REGISTRATION( MpdManagerTest );

int main(int argc, char* argv[])
{
    std::ofstream xmlout("MpdManagerTestResult.xml");
    CPPUNIT_NS::TextTestRunner runner;
    CPPUNIT_NS::XmlOutputter *outputter = new CPPUNIT_NS::XmlOutputter(&runner.result(), xmlout);

    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( "", false );
    outputter->write();

    return runner.result().wasSuccessful() ? 0 : 1;
}
