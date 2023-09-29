/* libBtfTest.cpp */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

#include "btf/btf.h"

#include <catch2/catch.hpp>

std::string readBtf(const std::string& path)
{
    std::string out;
    std::string line;
    std::ifstream in(path);
    while (in.good() && !in.eof())
    {
        getline(in, line);
        if (line.empty())
        {
            continue;
        }
        if (line.back() == '\r')
        {
            line = line.substr(0, line.size() - 1);
        }
        out += line;
        out += '\n';
    }
    return out;
}

TEST_CASE("Simple test (Source is Core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(300, "Core1", "Runnable1", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(400, "Core1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(500, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(600, "Core1", btf::Core::Events::idle));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(700, "Core2", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(800, "Core2", "Task2", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(900, "Core2", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(1000, "Core2", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(1100, "Core2", "Task2", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(1200, "Core2", btf::Core::Events::idle));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Task1,0,R,Runnable1,0,start\n"
                            "400,Task1,0,R,Runnable1,0,terminate\n"
                            "500,Core1,0,T,Task1,0,terminate\n"
                            "600,Core1,0,C,Core1,0,idle\n"
                            "700,Core2,0,C,Core2,0,execute\n"
                            "800,Core2,0,T,Task2,0,start\n"
                            "900,Task2,0,R,Runnable2,0,start\n"
                            "1000,Task2,0,R,Runnable2,0,terminate\n"
                            "1100,Core2,0,T,Task2,0,terminate\n"
                            "1200,Core2,0,C,Core2,0,idle\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, false, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Simple test (Source is Process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(300, "Task1", "Runnable1", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(400, "Task1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(500, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(600, "Core1", btf::Core::Events::idle));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(700, "Core2", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(800, "Core2", "Task2", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(900, "Task2", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(1000, "Task2", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(1100, "Core2", "Task2", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(1200, "Core2", btf::Core::Events::idle));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Task1,0,R,Runnable1,0,start\n"
                            "400,Task1,0,R,Runnable1,0,terminate\n"
                            "500,Core1,0,T,Task1,0,terminate\n"
                            "600,Core1,0,C,Core1,0,idle\n"
                            "700,Core2,0,C,Core2,0,execute\n"
                            "800,Core2,0,T,Task2,0,start\n"
                            "900,Task2,0,R,Runnable2,0,start\n"
                            "1000,Task2,0,R,Runnable2,0,terminate\n"
                            "1100,Core2,0,T,Task2,0,terminate\n"
                            "1200,Core2,0,C,Core2,0,idle\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, false, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Runnable preemption with auto suspend (Source is Core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(300, "Core1", "Runnable1", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(400, "Core1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(500, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(700, "Core1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(800, "Core1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(900, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(1000, "Core1", btf::Core::Events::idle));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Task1,0,R,Runnable1,0,start\n"
                            "400,Task1,0,R,Runnable1,0,suspend\n"
                            "400,Task1,0,R,Runnable2,0,start\n"
                            "500,Task1,0,R,Runnable2,0,suspend\n"
                            "500,Core1,0,T,Task1,0,preempt\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "600,Task1,0,R,Runnable2,0,resume\n"
                            "700,Task1,0,R,Runnable2,0,terminate\n"
                            "700,Task1,0,R,Runnable1,0,resume\n"
                            "800,Task1,0,R,Runnable1,0,terminate\n"
                            "900,Core1,0,T,Task1,0,terminate\n"
                            "1000,Core1,0,C,Core1,0,idle\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Runnable preemption with auto suspend (Source is Process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(300, "Task1", "Runnable1", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(400, "Task1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(500, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(700, "Task1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(800, "Task1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(900, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(1000, "Core1", btf::Core::Events::idle));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Task1,0,R,Runnable1,0,start\n"
                            "400,Task1,0,R,Runnable1,0,suspend\n"
                            "400,Task1,0,R,Runnable2,0,start\n"
                            "500,Task1,0,R,Runnable2,0,suspend\n"
                            "500,Core1,0,T,Task1,0,preempt\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "600,Task1,0,R,Runnable2,0,resume\n"
                            "700,Task1,0,R,Runnable2,0,terminate\n"
                            "700,Task1,0,R,Runnable1,0,resume\n"
                            "800,Task1,0,R,Runnable1,0,terminate\n"
                            "900,Core1,0,T,Task1,0,terminate\n"
                            "1000,Core1,0,C,Core1,0,idle\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Runnable preemption without auto suspend (source is Core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, false);

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(300, "Core1", "Runnable1", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(400, "Core1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(500, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(700, "Core1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(800, "Core1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(900, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(1000, "Core1", btf::Core::Events::idle));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Task1,0,R,Runnable1,0,start\n"
                            "400,Task1,0,R,Runnable2,0,start\n"
                            "500,Task1,0,R,Runnable2,0,suspend\n"
                            "500,Task1,0,R,Runnable1,0,suspend\n"
                            "500,Core1,0,T,Task1,0,preempt\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "600,Task1,0,R,Runnable1,0,resume\n"
                            "600,Task1,0,R,Runnable2,0,resume\n"
                            "700,Task1,0,R,Runnable2,0,terminate\n"
                            "800,Task1,0,R,Runnable1,0,terminate\n"
                            "900,Core1,0,T,Task1,0,terminate\n"
                            "1000,Core1,0,C,Core1,0,idle\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, false, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Runnable preemption without auto suspend (source is Process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, false, false);

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(300, "Task1", "Runnable1", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(400, "Task1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(500, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(700, "Task1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(800, "Task1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(900, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(1000, "Core1", btf::Core::Events::idle));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Task1,0,R,Runnable1,0,start\n"
                            "400,Task1,0,R,Runnable2,0,start\n"
                            "500,Task1,0,R,Runnable2,0,suspend\n"
                            "500,Task1,0,R,Runnable1,0,suspend\n"
                            "500,Core1,0,T,Task1,0,preempt\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "600,Task1,0,R,Runnable1,0,resume\n"
                            "600,Task1,0,R,Runnable2,0,resume\n"
                            "700,Task1,0,R,Runnable2,0,terminate\n"
                            "800,Task1,0,R,Runnable1,0,terminate\n"
                            "900,Core1,0,T,Task1,0,terminate\n"
                            "1000,Core1,0,C,Core1,0,idle\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, false, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Start of trace during runnable execution (source is Core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(300, "Core1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(400, "Core1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(500, "Core1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::terminate));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "300,Task1,0,R,Runnable2,0,start\n"
                            "400,Task1,0,R,Runnable2,0,terminate\n"
                            "500,Task1,0,R,Runnable1,0,terminate\n"
                            "600,Core1,0,T,Task1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Start of trace during runnable execution (source is Process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);

    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(300, "Task1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(400, "Task1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(500, "Task1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::terminate));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "300,Task1,0,R,Runnable2,0,start\n"
                            "400,Task1,0,R,Runnable2,0,terminate\n"
                            "500,Task1,0,R,Runnable1,0,terminate\n"
                            "600,Core1,0,T,Task1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("OS with auto task generation (source is core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf",btf::BtfFile::TimeScales::nano_seconds, true, true, false, true);

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(200, "Core2", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(300, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(400, "Core2", "Isr1", 0, btf::Process::Events::start,true));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(500, "Core1", "OsEvent2", btf::OS::Events::wait_event));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(600, "Core2", "OsEvent2", btf::OS::Events::set_event));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(700, "Core1", "OsEvent2", btf::OS::Events::clear_event));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(800, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(900, "Core2", "Isr1", 0, btf::Process::Events::terminate,true));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core2,0,C,Core2,0,execute\n"
                            "300,Core1,0,T,Task1,0,start\n"
                            "400,Core2,0,I,Isr1,0,start\n"
                            "500,Task1,0,EVENT,OsEvent2,0,wait_event\n"
                            "500,Core1,0,T,Task1,0,wait\n"
                            "600,Isr1,0,EVENT,OsEvent2,0,set_event\n"
                            "600,Core1,0,T,Task1,0,release\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "700,Task1,0,EVENT,OsEvent2,0,clear_event\n"
                            "800,Core1,0,T,Task1,0,terminate\n"
                            "900,Core2,0,I,Isr1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("OS with auto task generation (source is process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf",btf::BtfFile::TimeScales::nano_seconds, true, false, false, true);

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(200, "Core2", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(300, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(400, "Core2", "Isr1", 0, btf::Process::Events::start,true));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(500, "Task1", "OsEvent2", btf::OS::Events::wait_event));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(600, "Isr1", "OsEvent2", btf::OS::Events::set_event));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(700, "Task1", "OsEvent2", btf::OS::Events::clear_event));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(800, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(900, "Core2", "Isr1", 0, btf::Process::Events::terminate,true));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core2,0,C,Core2,0,execute\n"
                            "300,Core1,0,T,Task1,0,start\n"
                            "400,Core2,0,I,Isr1,0,start\n"
                            "500,Task1,0,EVENT,OsEvent2,0,wait_event\n"
                            "500,Core1,0,T,Task1,0,wait\n"
                            "600,Isr1,0,EVENT,OsEvent2,0,set_event\n"
                            "600,Core1,0,T,Task1,0,release\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "700,Task1,0,EVENT,OsEvent2,0,clear_event\n"
                            "800,Core1,0,T,Task1,0,terminate\n"
                            "900,Core2,0,I,Isr1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("OS without auto task generation (source is Core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(200, "Core2", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(300, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(400, "Core2", "Isr1", 0, btf::Process::Events::start,true));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(500, "Core1", "OsEvent2", btf::OS::Events::wait_event));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(500, "Core1", "Task1", 0, btf::Process::Events::wait));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(600, "Core2", "OsEvent2", btf::OS::Events::set_event));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::release));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(700, "Core1", "OsEvent2", btf::OS::Events::clear_event));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(800, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(900, "Core2", "Isr1", 0, btf::Process::Events::terminate,true));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core2,0,C,Core2,0,execute\n"
                            "300,Core1,0,T,Task1,0,start\n"
                            "400,Core2,0,I,Isr1,0,start\n"
                            "500,Task1,0,EVENT,OsEvent2,0,wait_event\n"
                            "500,Core1,0,T,Task1,0,wait\n"
                            "600,Isr1,0,EVENT,OsEvent2,0,set_event\n"
                            "600,Core1,0,T,Task1,0,release\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "700,Task1,0,EVENT,OsEvent2,0,clear_event\n"
                            "800,Core1,0,T,Task1,0,terminate\n"
                            "900,Core2,0,I,Isr1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("OS without auto task generation (source is Process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(200, "Core2", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(300, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(400, "Core2", "Isr1", 0, btf::Process::Events::start,true));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(500, "Task1", "OsEvent2", btf::OS::Events::wait_event));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(500, "Core1", "Task1", 0, btf::Process::Events::wait));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(600, "Isr1", "OsEvent2", btf::OS::Events::set_event));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::release));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.osEvent(700, "Task1", "OsEvent2", btf::OS::Events::clear_event));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(800, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(900, "Core2", "Isr1", 0, btf::Process::Events::terminate,true));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core2,0,C,Core2,0,execute\n"
                            "300,Core1,0,T,Task1,0,start\n"
                            "400,Core2,0,I,Isr1,0,start\n"
                            "500,Task1,0,EVENT,OsEvent2,0,wait_event\n"
                            "500,Core1,0,T,Task1,0,wait\n"
                            "600,Isr1,0,EVENT,OsEvent2,0,set_event\n"
                            "600,Core1,0,T,Task1,0,release\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "700,Task1,0,EVENT,OsEvent2,0,clear_event\n"
                            "800,Core1,0,T,Task1,0,terminate\n"
                            "900,Core2,0,I,Isr1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Stimulus", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.stimulusEvent(100, "StimuliCore1", "StimuliCore1", btf::Stimulus::Events::trigger));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.stimulusEvent(200, "StimuliTask", "StimuliTask", btf::Stimulus::Events::trigger));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "StimuliTask", "Task1", 0, btf::Process::Events::activate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(300, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(400, "Core1", btf::Core::Events::idle));
    REQUIRE(btf::ErrorCodes::success == btf.stimulusEvent(500, "StimuliCore2", "StimuliCore2", btf::Stimulus::Events::trigger));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(500, "Core2", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.stimulusEvent(600, "StimuliTask", "StimuliTask", btf::Stimulus::Events::trigger));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "StimuliTask", "Task1", 0, btf::Process::Events::activate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core2", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(700, "Core2", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(800, "Core2", btf::Core::Events::idle));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,StimuliCore1,0,STI,StimuliCore1,0,trigger\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,StimuliTask,0,STI,StimuliTask,0,trigger\n"
                            "200,StimuliTask,0,T,Task1,0,activate\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Core1,0,T,Task1,0,terminate\n"
                            "400,Core1,0,C,Core1,0,idle\n"
                            "500,StimuliCore2,0,STI,StimuliCore2,0,trigger\n"
                            "500,Core2,0,C,Core2,0,execute\n"
                            "600,StimuliTask,1,STI,StimuliTask,1,trigger\n"
                            "600,StimuliTask,1,T,Task1,0,activate\n"
                            "600,Core2,0,T,Task1,0,start\n"
                            "700,Core2,0,T,Task1,0,terminate\n"
                            "800,Core2,0,C,Core2,0,idle\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Scheduler (source is Core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.schedulerEvent(300, "Core1", "Scheduler1", btf::Scheduler::Events::schedulepoint));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(400, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::success == btf.schedulerEvent(500, "Scheduler1", "Scheduler1", btf::Scheduler::Events::schedule));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(700, "Core1", "Task1", 0, btf::Process::Events::terminate));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Task1,0,SCHED,Scheduler1,0,schedulepoint\n"
                            "400,Core1,0,T,Task1,0,preempt\n"
                            "500,Scheduler1,0,SCHED,Scheduler1,0,schedule\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "700,Core1,0,T,Task1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Scheduler (source is Process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.schedulerEvent(300, "Task1", "Scheduler1", btf::Scheduler::Events::schedulepoint));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(400, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::success == btf.schedulerEvent(500, "Scheduler1", "Scheduler1", btf::Scheduler::Events::schedule));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(600, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(700, "Core1", "Task1", 0, btf::Process::Events::terminate));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,Core1,0,T,Task1,0,start\n"
                            "300,Task1,0,SCHED,Scheduler1,0,schedulepoint\n"
                            "400,Core1,0,T,Task1,0,preempt\n"
                            "500,Scheduler1,0,SCHED,Scheduler1,0,schedule\n"
                            "600,Core1,0,T,Task1,0,resume\n"
                            "700,Core1,0,T,Task1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Task state transitions", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::allocated_to_different_core == btf.processEvent(100, "Core2", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::descending_timestamp == btf.processEvent(0, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::invalid_state_transition == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::multiple_tasks_running == btf.processEvent(100, "Core1", "Task2", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::poll));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::run));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::wait));
    REQUIRE(btf::ErrorCodes::invalid_state_transition == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::release));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::invalid_state_transition == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::poll_parking));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::terminate));
    btf.finish();
}

TEST_CASE("Task interrupt suspended", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.coreEvent(100, "Core1", btf::Core::Events::execute));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(200, "StimuliIsr", "Isr1", 0, btf::Process::Events::activate,true));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(300, "Scheduler1", "Isr1", 0, btf::Process::Events::interrupt_suspended,true));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,C,Core1,0,execute\n"
                            "200,StimuliIsr,0,I,Isr1,0,activate\n"
                            "300,Scheduler1,0,I,Isr1,0,interrupt_suspended\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Process Runnable interaction (source is Core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable1", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable3", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable4", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::no_task_running == btf.runnableEvent(100, "Core1", "Runnable4", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task2", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::runnable_source_task_not_running == btf.runnableEvent(100, "Core1", "Runnable4", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task2", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable4", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable3", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::terminate_on_task_with_running_runnables == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Core1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::terminate));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,T,Task1,0,start\n"
                            "100,Task1,0,R,Runnable1,0,start\n"
                            "100,Task1,0,R,Runnable1,0,suspend\n"
                            "100,Task1,0,R,Runnable2,0,start\n"
                            "100,Task1,0,R,Runnable2,0,suspend\n"
                            "100,Task1,0,R,Runnable3,0,start\n"
                            "100,Task1,0,R,Runnable3,0,suspend\n"
                            "100,Task1,0,R,Runnable4,0,start\n"
                            "100,Task1,0,R,Runnable4,0,suspend\n"
                            "100,Task1,0,R,Runnable2,1,start\n"
                            "100,Task1,0,R,Runnable2,1,terminate\n"
                            "100,Task1,0,R,Runnable4,0,resume\n"
                            "100,Task1,0,R,Runnable4,0,suspend\n"
                            "100,Task1,0,R,Runnable2,2,start\n"
                            "100,Task1,0,R,Runnable2,2,suspend\n"
                            "100,Core1,0,T,Task1,0,preempt\n"
                            "100,Core1,0,T,Task2,0,start\n"
                            "100,Core1,0,T,Task2,0,preempt\n"
                            "100,Core1,0,T,Task1,0,resume\n"
                            "100,Task1,0,R,Runnable2,2,resume\n"
                            "100,Task1,0,R,Runnable2,2,terminate\n"
                            "100,Task1,0,R,Runnable4,0,resume\n"
                            "100,Task1,0,R,Runnable4,0,terminate\n"
                            "100,Task1,0,R,Runnable3,0,resume\n"
                            "100,Task1,0,R,Runnable3,0,terminate\n"
                            "100,Task1,0,R,Runnable2,0,resume\n"
                            "100,Task1,0,R,Runnable2,0,terminate\n"
                            "100,Task1,0,R,Runnable1,0,resume\n"
                            "100,Task1,0,R,Runnable1,0,terminate\n"
                            "100,Core1,0,T,Task1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Process Runnable interaction (source is Process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);

    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable1", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable3", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable4", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable2", btf::Runnable::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::no_task_running == btf.runnableEvent(100, "Task1", "Runnable4", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task2", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::runnable_source_task_not_running == btf.runnableEvent(100, "Task1", "Runnable4", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task2", 0, btf::Process::Events::preempt));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable4", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable3", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable2", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::terminate_on_task_with_running_runnables == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.runnableEvent(100, "Task1", "Runnable1", btf::Runnable::Events::terminate));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::terminate));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,T,Task1,0,start\n"
                            "100,Task1,0,R,Runnable1,0,start\n"
                            "100,Task1,0,R,Runnable1,0,suspend\n"
                            "100,Task1,0,R,Runnable2,0,start\n"
                            "100,Task1,0,R,Runnable2,0,suspend\n"
                            "100,Task1,0,R,Runnable3,0,start\n"
                            "100,Task1,0,R,Runnable3,0,suspend\n"
                            "100,Task1,0,R,Runnable4,0,start\n"
                            "100,Task1,0,R,Runnable4,0,suspend\n"
                            "100,Task1,0,R,Runnable2,1,start\n"
                            "100,Task1,0,R,Runnable2,1,terminate\n"
                            "100,Task1,0,R,Runnable4,0,resume\n"
                            "100,Task1,0,R,Runnable4,0,suspend\n"
                            "100,Task1,0,R,Runnable2,2,start\n"
                            "100,Task1,0,R,Runnable2,2,suspend\n"
                            "100,Core1,0,T,Task1,0,preempt\n"
                            "100,Core1,0,T,Task2,0,start\n"
                            "100,Core1,0,T,Task2,0,preempt\n"
                            "100,Core1,0,T,Task1,0,resume\n"
                            "100,Task1,0,R,Runnable2,2,resume\n"
                            "100,Task1,0,R,Runnable2,2,terminate\n"
                            "100,Task1,0,R,Runnable4,0,resume\n"
                            "100,Task1,0,R,Runnable4,0,terminate\n"
                            "100,Task1,0,R,Runnable3,0,resume\n"
                            "100,Task1,0,R,Runnable3,0,terminate\n"
                            "100,Task1,0,R,Runnable2,0,resume\n"
                            "100,Task1,0,R,Runnable2,0,terminate\n"
                            "100,Task1,0,R,Runnable1,0,resume\n"
                            "100,Task1,0,R,Runnable1,0,terminate\n"
                            "100,Core1,0,T,Task1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Task migration", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::invalid_state_transition == btf.taskMigrationEvent(100, "Core1", "Core2", "Task1", 0));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::wait));
    REQUIRE(btf::ErrorCodes::success == btf.taskMigrationEvent(100, "Core1", "Core2", "Task1", 0));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core2", "Task1", 0, btf::Process::Events::release));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core2", "Task1", 0, btf::Process::Events::resume));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core2", "Task1", 0, btf::Process::Events::terminate));
    btf.finish();

    auto data = readBtf("test.btf");

    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "100,Core1,0,T,Task1,0,start\n"
                            "100,Core1,0,T,Task1,0,wait\n"
                            "100,Core1,0,T,Task1,0,enforcedmigration\n"
                            "100,Core2,0,T,Task1,0,fullmigration\n"
                            "100,Core2,0,T,Task1,0,release\n"
                            "100,Core2,0,T,Task1,0,resume\n"
                            "100,Core2,0,T,Task1,0,terminate\n";

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Semaphore (source is Core)", "[libBtf]")
{
    btf::BtfFile btf("test.btf");

    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(0, "Sem1", "Sem1", btf::Semaphore::Events::free, 0));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(150, "Core2", "Isr1", 0, btf::Process::Events::start,true));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Core1", "Sem1", btf::Semaphore::Events::requestsemaphore, 0));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Core1", "Sem1", btf::Semaphore::Events::increment, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Core1", "Sem1", btf::Semaphore::Events::queued, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Sem1", "Sem1", btf::Semaphore::Events::lock, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Core1", "Sem1", btf::Semaphore::Events::assigned, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Core2", "Sem1", btf::Semaphore::Events::requestsemaphore, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Core2", "Sem1", btf::Semaphore::Events::increment, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Core2", "Sem1", btf::Semaphore::Events::queued, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Sem1", "Sem1", btf::Semaphore::Events::overfull, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Core2", "Sem1", btf::Semaphore::Events::waiting, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(400, "Core1", "Sem1", btf::Semaphore::Events::released, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(400, "Core1", "Sem1", btf::Semaphore::Events::decrement, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(400, "Sem1", "Sem1", btf::Semaphore::Events::full, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(400, "Core2", "Sem1", btf::Semaphore::Events::assigned, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(500, "Core2", "Sem1", btf::Semaphore::Events::decrement, 0));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(500, "Sem1", "Sem1", btf::Semaphore::Events::unlock, 0));
    btf.finish();

    auto data = readBtf("test.btf");
    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "0,Sem1,0,SEM,Sem1,0,free,0\n"
                            "100,Core1,0,T,Task1,0,start\n"
                            "150,Core2,0,I,Isr1,0,start\n"
                            "200,Task1,0,SEM,Sem1,0,requestsemaphore,0\n"
                            "200,Task1,0,SEM,Sem1,0,increment,1\n"
                            "200,Task1,0,SEM,Sem1,0,queued,1\n"
                            "200,Sem1,0,SEM,Sem1,0,lock,1\n"
                            "200,Task1,0,SEM,Sem1,0,assigned,1\n"
                            "300,Isr1,0,SEM,Sem1,0,requestsemaphore,1\n"
                            "300,Isr1,0,SEM,Sem1,0,increment,2\n"
                            "300,Isr1,0,SEM,Sem1,0,queued,2\n"
                            "300,Sem1,0,SEM,Sem1,0,overfull,2\n"
                            "300,Isr1,0,SEM,Sem1,0,waiting,2\n"
                            "400,Task1,0,SEM,Sem1,0,released,2\n"
                            "400,Task1,0,SEM,Sem1,0,decrement,1\n"
                            "400,Sem1,0,SEM,Sem1,0,full,1\n"
                            "400,Isr1,0,SEM,Sem1,0,assigned,1\n"
                            "500,Isr1,0,SEM,Sem1,0,decrement,0\n"
                            "500,Sem1,0,SEM,Sem1,0,unlock,0\n";
;

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Semaphore (source is Process)", "[libBtf]")
{
    btf::BtfFile btf("test.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);

    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(0, "Sem1", "Sem1", btf::Semaphore::Events::free, 0));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(100, "Core1", "Task1", 0, btf::Process::Events::start));
    REQUIRE(btf::ErrorCodes::success == btf.processEvent(150, "Core2", "Isr1", 0, btf::Process::Events::start,true));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Task1", "Sem1", btf::Semaphore::Events::requestsemaphore, 0));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Task1", "Sem1", btf::Semaphore::Events::increment, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Task1", "Sem1", btf::Semaphore::Events::queued, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Sem1", "Sem1", btf::Semaphore::Events::lock, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(200, "Task1", "Sem1", btf::Semaphore::Events::assigned, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Isr1", "Sem1", btf::Semaphore::Events::requestsemaphore, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Isr1", "Sem1", btf::Semaphore::Events::increment, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Isr1", "Sem1", btf::Semaphore::Events::queued, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Sem1", "Sem1", btf::Semaphore::Events::overfull, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(300, "Isr1", "Sem1", btf::Semaphore::Events::waiting, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(400, "Task1", "Sem1", btf::Semaphore::Events::released, 2));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(400, "Task1", "Sem1", btf::Semaphore::Events::decrement, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(400, "Sem1", "Sem1", btf::Semaphore::Events::full, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(400, "Isr1", "Sem1", btf::Semaphore::Events::assigned, 1));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(500, "Isr1", "Sem1", btf::Semaphore::Events::decrement, 0));
    REQUIRE(btf::ErrorCodes::success == btf.semaphoreEvent(500, "Sem1", "Sem1", btf::Semaphore::Events::unlock, 0));
    btf.finish();

    auto data = readBtf("test.btf");
    std::string should_be = "#version 2.2.1\n"
                            "#creator libBtf\n"
                            "#timescale ns\n"
                            "0,Sem1,0,SEM,Sem1,0,free,0\n"
                            "100,Core1,0,T,Task1,0,start\n"
                            "150,Core2,0,I,Isr1,0,start\n"
                            "200,Task1,0,SEM,Sem1,0,requestsemaphore,0\n"
                            "200,Task1,0,SEM,Sem1,0,increment,1\n"
                            "200,Task1,0,SEM,Sem1,0,queued,1\n"
                            "200,Sem1,0,SEM,Sem1,0,lock,1\n"
                            "200,Task1,0,SEM,Sem1,0,assigned,1\n"
                            "300,Isr1,0,SEM,Sem1,0,requestsemaphore,1\n"
                            "300,Isr1,0,SEM,Sem1,0,increment,2\n"
                            "300,Isr1,0,SEM,Sem1,0,queued,2\n"
                            "300,Sem1,0,SEM,Sem1,0,overfull,2\n"
                            "300,Isr1,0,SEM,Sem1,0,waiting,2\n"
                            "400,Task1,0,SEM,Sem1,0,released,2\n"
                            "400,Task1,0,SEM,Sem1,0,decrement,1\n"
                            "400,Sem1,0,SEM,Sem1,0,full,1\n"
                            "400,Isr1,0,SEM,Sem1,0,assigned,1\n"
                            "500,Isr1,0,SEM,Sem1,0,decrement,0\n"
                            "500,Sem1,0,SEM,Sem1,0,unlock,0\n";
;

    REQUIRE(should_be == data);

    //Now let's test, if the import feature also works for this case.
    //Caution: Here the source must be according to the spec.
    btf::BtfFile importbtf("importtest.btf", btf::BtfFile::TimeScales::nano_seconds, true, false);
    importbtf.importFromFile("test.btf");
    importbtf.finish();
    data = readBtf("importtest.btf");
    REQUIRE(should_be == data);
}

TEST_CASE("Logging", "[libhelper]")
{
    helper::logging::initLogging(helper::logging::LogLevel::trace, "log.txt");
    helper::logging::printTrace() << "This is a trace message" << '\n';
    helper::logging::printWarning() << "This is a warning message" << '\n';
    helper::logging::printError() << "This is an error message" << '\n';
    helper::logging::flush();

    auto data = readBtf("log.txt");

    std::string should_be = "Trace [file: test\\libBtfTest.cpp: 1047 `____C_A_T_C_H____T_E_S_T____44`] This is a trace message\n"
                            "Warning [file: test\\libBtfTest.cpp: 1048 `____C_A_T_C_H____T_E_S_T____44`] This is a warning message\n"
                            "Error [file: test\\libBtfTest.cpp: 1049 `____C_A_T_C_H____T_E_S_T____44`] This is an error message\n";

    REQUIRE(should_be == data);
}
