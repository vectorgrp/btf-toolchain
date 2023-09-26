/* libbtf_binding.cpp */

/* Copyright (c) 2023 Vector Informatik GmbH

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */


#include "btf/btf.h"
#include "libhelper_binding.h"

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

/**
 * @brief Macro that creates a function that will be called when the module pybtf is imported within python.
 * It enables the creation of Python bindings for the C++ code of the BTF lib.
 * @param[in] name Name of the python module the C++ source code is linked to.
 * @param[in] variable Module interface for creating binding code.
*/
PYBIND11_MODULE(pybtf, m) // NOLINT
{
    m.doc() = "utility library for BTF file writing";

    py::class_<btf::BtfEntry> btfEntry(m, "BtfEntry");
    btfEntry.def_readwrite("time_", &btf::BtfEntry::time_)
        .def_readwrite("source_hash_", &btf::BtfEntry::source_hash_)
        .def_readwrite("source_instance_", &btf::BtfEntry::source_instance_)
        .def_readwrite("target_hash_", &btf::BtfEntry::target_hash_)
        .def_readwrite("target_instance_", &btf::BtfEntry::target_instance_)
        .def_readwrite("event_", &btf::BtfEntry::event_)
        .def_readwrite("note_", &btf::BtfEntry::note_)
        .def("toString", &btf::BtfEntry::toString, "Converts the BTF entry to string", py::arg("hash_map"));

    py::enum_<btf::ErrorCodes>(m, "ErrorCodes")
        .value("success", btf::ErrorCodes::success)
        .value("already_in_state", btf::ErrorCodes::already_in_state)
        .value("invalid_state_transition", btf::ErrorCodes::invalid_state_transition)
        .value("descending_timestamp", btf::ErrorCodes::descending_timestamp)
        .value("invalid_type", btf::ErrorCodes::invalid_type)
        .value("invalid_event", btf::ErrorCodes::invalid_event)
        .value("core_idle_task_still_running", btf::ErrorCodes::core_idle_task_still_running)
        .value("multiple_tasks_running", btf::ErrorCodes::multiple_tasks_running)
        .value("event_on_idle_core", btf::ErrorCodes::event_on_idle_core)
        .value("runnable_source_task_not_running", btf::ErrorCodes::runnable_source_task_not_running)
        .value("no_task_running", btf::ErrorCodes::no_task_running)
        .value("terminate_on_runnable_with_running_sub_runnable", btf::ErrorCodes::terminate_on_runnable_with_running_sub_runnable)
        .value("terminate_on_task_with_running_runnables", btf::ErrorCodes::terminate_on_task_with_running_runnables)
        .value("allocated_to_different_core", btf::ErrorCodes::allocated_to_different_core)
        .value("source_and_target_not_equal", btf::ErrorCodes::source_and_target_not_equal)
        .value("amount_of_semaphore_accesses_invalid", btf::ErrorCodes::amount_of_semaphore_accesses_invalid);
    m.def("errorCodeToString", &btf::errorCodeToString, "converts an error code to string", py::arg("code"));

    py::enum_<btf::Core::Events>(m, "CoreEvent")
        .value("idle", btf::Core::Events::idle)
        .value("execute", btf::Core::Events::execute)
        .value("set_frequence", btf::Core::Events::set_frequence)
        .value("unknown", btf::Core::Events::unknown);

    py::enum_<btf::OS::Events>(m, "OsEvent")
        .value("clear_event", btf::OS::Events::clear_event)
        .value("set_event", btf::OS::Events::set_event)
        .value("wait_event", btf::OS::Events::wait_event)
        .value("unknown", btf::OS::Events::unknown);

    py::enum_<btf::Process::Events>(m, "ProcessEvent")
        .value("activate", btf::Process::Events::activate)
        .value("start", btf::Process::Events::start)
        .value("preempt", btf::Process::Events::preempt)
        .value("resume", btf::Process::Events::resume)
        .value("terminate", btf::Process::Events::terminate)
        .value("poll", btf::Process::Events::poll)
        .value("run", btf::Process::Events::run)
        .value("park", btf::Process::Events::park)
        .value("poll_parking", btf::Process::Events::poll_parking)
        .value("release_parking", btf::Process::Events::release_parking)
        .value("wait", btf::Process::Events::wait)
        .value("release", btf::Process::Events::release)
        .value("full_migration", btf::Process::Events::full_migration)
        .value("enforced_migration", btf::Process::Events::enforced_migration)
        .value("interrupt_suspended", btf::Process::Events::interrupt_suspended)
        .value("mtalimitexceeded", btf::Process::Events::mtalimitexceeded)
        .value("unknown", btf::Process::Events::unknown);

    py::enum_<btf::Runnable::Events>(m, "RunnableEvent")
        .value("start", btf::Runnable::Events::start)
        .value("terminate", btf::Runnable::Events::terminate)
        .value("suspend", btf::Runnable::Events::suspend)
        .value("resume", btf::Runnable::Events::resume)
        .value("unknown", btf::Runnable::Events::unknown);

    py::enum_<btf::Scheduler::Events>(m, "SchedulerEvent")
        .value("schedule", btf::Scheduler::Events::schedule)
        .value("schedulepoint", btf::Scheduler::Events::schedulepoint)
        .value("unknown", btf::Scheduler::Events::unknown);

    py::enum_<btf::Semaphore::Events>(m, "SemaphoreEvent")
        .value("assigned", btf::Semaphore::Events::assigned)
        .value("decrement", btf::Semaphore::Events::decrement)
        .value("free", btf::Semaphore::Events::free)
        .value("full", btf::Semaphore::Events::full)
        .value("increment", btf::Semaphore::Events::increment)
        .value("lock", btf::Semaphore::Events::lock)
        .value("lock_used", btf::Semaphore::Events::lock_used)
        .value("overfull", btf::Semaphore::Events::overfull)
        .value("queued", btf::Semaphore::Events::queued)
        .value("released", btf::Semaphore::Events::released)
        .value("requestsemaphore", btf::Semaphore::Events::requestsemaphore)
        .value("unlock", btf::Semaphore::Events::unlock)
        .value("unlock_full", btf::Semaphore::Events::unlock_full)
        .value("used", btf::Semaphore::Events::used)
        .value("waiting", btf::Semaphore::Events::waiting)
        .value("unknown", btf::Semaphore::Events::unknown);

    py::enum_<btf::Signal::Events>(m, "SignalEvent")
        .value("read", btf::Signal::Events::read)
        .value("write", btf::Signal::Events::write)
        .value("unknown", btf::Signal::Events::unknown);

    py::enum_<btf::Simulation::Events>(m, "SimulationEvent")
        .value("tag", btf::Simulation::Events::tag)
        .value("unknown", btf::Simulation::Events::unknown);
     
    py::enum_<btf::Stimulus::Events>(m, "StimulusEvent")
        .value("trigger", btf::Stimulus::Events::trigger)
        .value("unknown", btf::Stimulus::Events::unknown);

    py::class_<btf::BtfFile> btfFile(m, "BtfFile");

    py::enum_<btf::BtfFile::TimeScales>(btfFile, "Timescale")
        .value("pico_seconds", btf::BtfFile::TimeScales::pico_seconds)
        .value("nano_seconds", btf::BtfFile::TimeScales::nano_seconds)
        .value("micro_seconds", btf::BtfFile::TimeScales::micro_seconds)
        .value("milli_seconds", btf::BtfFile::TimeScales::milli_seconds);

    btfFile.def(py::init<std::string, btf::BtfFile::TimeScales, bool, bool, bool, bool>())
        .def("finish", static_cast<void (btf::BtfFile::*)()>(&btf::BtfFile::finish), "write the BTF to file")
        .def("importFromFile", &btf::BtfFile::importFromFile, "Appends the data from a BTF file. For now only importing into a empty BTF is supported.",
             py::arg("path"), py::arg("delimiter") = ',')
        .def("setStringHashMap", &btf::BtfFile::setStringHashMap,
             "set the id name translation map. Be carefully using this with events that uses the names instead of ids", py::arg("hash_map"))
        .def("coreEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, btf::Core::Events)>(&btf::BtfFile::coreEvent),
             "emit a core event", py::arg("time"), py::arg("core"), py::arg("core_event"))
        .def("coreEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, btf::Core::Events)>(&btf::BtfFile::coreEvent), "emit a core event",
             py::arg("time"), py::arg("core_hash"), py::arg("core_event"))
        .def("osEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&, btf::OS::Events)>(&btf::BtfFile::osEvent), "emit an os event",
             py::arg("time"), py::arg("process"), py::arg("os"), py::arg("os_event")) 
        .def("osEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, size_t, btf::OS::Events)>(&btf::BtfFile::osEvent), "emit an os event",
             py::arg("time"), py::arg("core_hash"), py::arg("os_hash"), py::arg("os_event"))          
        .def("taskMigrationEvent",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&, const std::string&, uint64_t)>(
                 &btf::BtfFile::taskMigrationEvent),
             "emit a task migration event", py::arg("time"), py::arg("source_core"), py::arg("destination_core"), py::arg("task"), py::arg("task_instance_id"))
        .def("taskMigrationEvent",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, size_t, size_t, uint64_t)>(&btf::BtfFile::taskMigrationEvent),
             "emit a task migration event", py::arg("time"), py::arg("source_core_hash"), py::arg("destination_core_hash"), py::arg("task_hash"),
             py::arg("task_instance_id"))
        .def("processEvent",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&, uint64_t, btf::Process::Events, bool)>(
                 &btf::BtfFile::processEvent),
             "emit a process event (except task migration).", py::arg("time"), py::arg("source"), py::arg("process"), py::arg("process_instance_id"), py::arg("process_event"),
             py::arg("is_isr"))
        .def("processEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, size_t, uint64_t, btf::Process::Events, bool)>(&btf::BtfFile::processEvent),
             "emit a task event (except migration).", py::arg("time"), py::arg("source_hash"), py::arg("process_hash"), py::arg("process_instance_id"),
             py::arg("process_event"), py::arg("is_isr"))
        .def("runnableEvent",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&, btf::Runnable::Events)>(
                 &btf::BtfFile::runnableEvent),
             "emit a runnable event (source is a process)", py::arg("time"), py::arg("process"), py::arg("runnable"), py::arg("runnable_event"))
        .def("runnableEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, size_t, size_t, btf::Runnable::Events)>(&btf::BtfFile::runnableEvent),
             "emit a runnable event (source is a process)", py::arg("time"), py::arg("core_hash"), py::arg("process_hash"), py::arg("runnable_hash"), py::arg("runnable_event"))
        .def("semaphoreEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&, btf::Semaphore::Events, uint64_t)>(&btf::BtfFile::semaphoreEvent),
             "emit a semaphore event.", py::arg("time"), py::arg("source"), py::arg("target"), py::arg("semaphore_event"), py::arg("note"))
        .def("semaphoreEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, size_t, btf::Semaphore::Events, uint64_t)>(&btf::BtfFile::semaphoreEvent),
             "emit a semaphore event.", py::arg("time"), py::arg("core_hash"), py::arg("semaphore_hash"), py::arg("semaphore_event"), py::arg("note"))
        .def("semaphoreEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, btf::Semaphore::Events, uint64_t)>(&btf::BtfFile::semaphoreEvent),
             "emit a semaphore event.", py::arg("time"), py::arg("semaphore_hash"), py::arg("semaphore_event"), py::arg("note"))
        .def("schedulerEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&, btf::Scheduler::Events)>(&btf::BtfFile::schedulerEvent),
             "emit a scheduler event.", py::arg("time"), py::arg("source"), py::arg("scheduler"), py::arg("scheduler_event"))
        .def("schedulerEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, btf::Scheduler::Events)>(&btf::BtfFile::schedulerEvent),
             "emit a scheduler event.", py::arg("time"), py::arg("scheduler_hash"), py::arg("scheduler_event"))
        .def("schedulerEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, size_t, btf::Scheduler::Events)>(&btf::BtfFile::schedulerEvent),
             "emit a scheduler event.", py::arg("time"), py::arg("core_hash"), py::arg("scheduler_hash"), py::arg("scheduler_event"))
        .def("signalEvent",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&, btf::Signal::Events, const std::string&)>(
                 &btf::BtfFile::signalEvent),
             "emit a signal event (source is a task).", py::arg("time"), py::arg("task"), py::arg("signal"), py::arg("signal_event"),
             py::arg("signal_value") = "")
        .def("signalEvent",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, size_t, btf::Signal::Events, const std::string&)>(&btf::BtfFile::signalEvent),
             "emit a signal event (source is a task).", py::arg("time"), py::arg("core_hash"), py::arg("signal_hash"), py::arg("signal_event"),
             py::arg("signal_value") = "")
        .def("stimulusEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&, btf::Stimulus::Events)>(&btf::BtfFile::stimulusEvent),
             "emit a stimulus event.", py::arg("time"), py::arg("source"), py::arg("target"), py::arg("stimulus_event"))
        .def("stimulusEvent", static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, btf::Stimulus::Events)>(&btf::BtfFile::stimulusEvent),
             "emit a stimulus event.", py::arg("time"), py::arg("stimulus_hash"), py::arg("stimulus_event"))
        //.def("getEventsForEntity",
        //     static_cast<std::list<std::list<btf::BtfEntry>::iterator> (btf::BtfFile::*)(const std::string&)>(
        //         &btf::BtfFile::getEventsForEntity),
        //     "Gets all events for a entity", py::arg("entity"))
        //.def("getEventsForEntity",
        //     static_cast<std::list<std::list<btf::BtfEntry>::iterator> (btf::BtfFile::*)(size_t)>(
        //         &btf::BtfFile::getEventsForEntity),
        //     "Gets all events for a entity", py::arg("entity_hash"))
        .def("comment", &btf::BtfFile::comment, "emits a comment", py::arg("comment"))
        .def("headerEntry", &btf::BtfFile::headerEntry, "writes a custom entry to the header", py::arg("header_entry"))
        //.def("insertEvent", &btf::BtfFile::insertEvent,
        //     "inserts a event into the trace. Warning: use on own risk, no checks are performed, can lead to invalid "
        //     "BTF",
        //     py::arg("pos"), py::arg("entry"))
        //.def("getEntityEvents", &btf::BtfFile::getEntityEvents, "returns the list of events for a entity",
        //     py::arg("entity"))
        .def("getAllEvents", &btf::BtfFile::getAllEvents, "returns a list of all events")
        .def("getNumberOfAllEvents", &btf::BtfFile::getNumberOfAllEvents, "returns the number of all events")
        .def("simulationEventProcessName",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&)>(&btf::BtfFile::simulationEventProcessName),
             "emits a process name event (e.g. name change or initial name)", py::arg("time"), py::arg("process"), py::arg("name"))
        .def("simulationEventProcessName",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, const std::string&)>(&btf::BtfFile::simulationEventProcessName),
             "emits a process name event (e.g. name change or initial name)", py::arg("time"), py::arg("process_hash"), py::arg("name"))
        .def("simulationEventProcessCreation",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, uint64_t, uint64_t)>(&btf::BtfFile::simulationEventProcessCreation),
             "emits a process creation event, e.g. PID and PPID mapping to a process", py::arg("time"), py::arg("process"), py::arg("pid"), py::arg("ppid"))
        .def("simulationEventProcessCreation",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, uint64_t, uint64_t)>(&btf::BtfFile::simulationEventProcessCreation),
             "emits a process creation event, e.g. PID and PPID mapping to a process", py::arg("time"), py::arg("process_hash"), py::arg("pid"),
             py::arg("ppid"))
        .def("simulationEventThreadName",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, const std::string&)>(&btf::BtfFile::simulationEventThreadName),
             "emits a thread name event (e.g. name change or initial name)", py::arg("time"), py::arg("thread"), py::arg("name"))
        .def("simulationEventThreadName",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, const std::string&)>(&btf::BtfFile::simulationEventThreadName),
             "emits a thread name event (e.g. name change or initial name)", py::arg("time"), py::arg("thread_hash"), py::arg("name"))
        .def("simulationEventThreadCreation",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, const std::string&, uint64_t, uint64_t)>(&btf::BtfFile::simulationEventThreadCreation),
             "emits a thread creation event, e.g. TID and PID mapping to a thread", py::arg("time"), py::arg("thread"), py::arg("tid"), py::arg("pid"))
        .def("simulationEventThreadCreation",
             static_cast<btf::ErrorCodes (btf::BtfFile::*)(uint64_t, size_t, uint64_t, uint64_t)>(&btf::BtfFile::simulationEventThreadCreation),
             "emits a thread creation event, e.g. TID and PID mapping to a thread", py::arg("time"), py::arg("thread_hash"), py::arg("tid"), py::arg("pid"))
        .def("setIgnoreMultipleTaskReleases", &btf::BtfFile::setIgnoreMultipleTaskReleases, "sets the option to ignore multiple release events on tasks", py::arg("value"));

    init_pybtfhelper(m);
}
