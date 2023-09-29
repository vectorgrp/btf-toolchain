/* btf.cpp */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

#include "btf/btf.h"

#include "helper/helper.h"

using helper::logging::printTrace;
using helper::logging::printWarning;

namespace btf
{
// cppcheck-suppress constParameter
std::string BtfEntry::toString(std::unordered_map<size_t, std::string>& hash_map) const
{
    std::stringstream ret;
    if (type_ == EntityTypes::comment)
    {
        ret << "# ";
        // remove newlines
        for (const auto& c : note_)
        {
            if (c != '\r' && c != '\n')
            {
                ret << c;
            }
        }
    }
    else
    {
        ret << time_ << "," << hash_map[source_hash_] << "," << source_instance_ << "," << entityTypeToString(type_) << ",";
        ret << hash_map[target_hash_] << "," << target_instance_ << "," << eventToString();

        // notes are possible for:
        //  - target signal and event write
        //  - simulation events
        //  - semaphores
        if ((type_ == EntityTypes::simulation) || (type_ == EntityTypes::signal && event_.signal_event == Signal::Events::write) || type_ == EntityTypes::semaphore)
        {
            if (!note_.empty())
            {
                ret << ",";
                // remove newlines
                for (const auto& c : note_)
                {
                    if (c != '\r' && c != '\n')
                    {
                        ret << c;
                    }
                }
            }
        }
    }
    return ret.str();
}

std::string BtfEntry::eventToString() const
{
    switch (type_)
    {
    case EntityTypes::core:
        return Core::eventToString(event_.core_event);
    case EntityTypes::os:
        return OS::eventToString(event_.os_event);
    case EntityTypes::task:
    case EntityTypes::isr:
    case EntityTypes::thread:
        return Process::eventToString(event_.process_event);
    case EntityTypes::runnable:
    case EntityTypes::syscall:
        return Runnable::eventToString(event_.runnable_event);
    case EntityTypes::scheduler:
        return Scheduler::eventToString(event_.scheduler_event);
    case EntityTypes::semaphore:
        return Semaphore::eventToString(event_.semaphore_event);
    case EntityTypes::signal:
        return Signal::eventToString(event_.signal_event);
    case EntityTypes::simulation:
        return Simulation::eventToString(event_.simulation_event);
    case EntityTypes::stimulus:
        return Stimulus::eventToString(event_.stimulus_event);
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown type")
    }
}

BtfFile::BtfFile(std::string path, TimeScales time_scale, bool auto_suspend_parent_runnable, bool source_is_core, bool auto_generate_core_events, bool auto_wait_resume_os_events)
    : path_(std::move(path)), time_scale_(time_scale), auto_suspend_parent_runnable_(auto_suspend_parent_runnable), source_is_core_(source_is_core), auto_generate_core_events_(auto_generate_core_events),
       auto_wait_resume_os_events_(auto_wait_resume_os_events)

{
}

void BtfFile::importFromFile(const std::string& path, char delimiter)
{
    // disable auto generating events
    auto_generate_events_ = false;
    
    std::ifstream file(path);

    if (!file.is_open() || !file.good())
    {
        throw std::runtime_error("could not open file");
    }

    std::string line;

    bool is_waiting_for_full_migration_event{ false };
    std::string migration_source_core{};
    std::string migration_task{};

    while (file.good())
    {
        helper::util::getline(file, line);
        if (line.empty())
        {
            continue;
        }

        // handle header and comments => keep comments and remove header
        if (line[0] == '#')
        {   
            if(line.substr(1,7) == "version" || line.substr(1,7) == "creator" || line.substr(1,9) == "timescale")
            {
                continue;
            }
            else
            {
            comment(line.substr(1));
            continue;
            }
        }

        // make sure we do not have \r at the end
        if (line.back() == '\r')
        {
            line.pop_back();
        }

        // read time, source, source_instance_id, type, target, target_instance_id, event and note
        auto time_end = line.find_first_of(delimiter);
        auto source_end = line.find_first_of(delimiter, time_end + 1);
        auto source_instance_id_end = line.find_first_of(delimiter, source_end + 1);
        auto type_end = line.find_first_of(delimiter, source_instance_id_end + 1);
        auto target_end = line.find_first_of(delimiter, type_end + 1);
        auto target_instance_id_end = line.find_first_of(delimiter, target_end + 1);
        auto event_end = line.find_first_of(delimiter, target_instance_id_end + 1);
        auto note_end = line.find_first_of(delimiter, event_end +1);

        // event end and note_end is allowed to be npos
        if (time_end == std::string::npos || source_end == std::string::npos || source_instance_id_end == std::string::npos || type_end == std::string::npos ||
            target_end == std::string::npos || target_instance_id_end == std::string::npos)
        {
            printWarning() << "Could not import line: " << line << ": invalid format\n";
            continue;
        }

        auto time_str = line.substr(0, time_end);
        auto source = line.substr(time_end + 1, source_end - time_end - 1);
        auto sid_str = line.substr(source_end + 1, source_instance_id_end - source_end - 1);
        auto type_str = line.substr(source_instance_id_end + 1, type_end - source_instance_id_end - 1);
        auto target = line.substr(type_end + 1, target_end - type_end - 1);
        auto tid_str = line.substr(target_end + 1, target_instance_id_end - target_end - 1);
        std::string event_str;
        if (event_end == std::string::npos)
        {
            event_str = line.substr(target_instance_id_end + 1);
        }
        else
        {
            event_str = line.substr(target_instance_id_end + 1, event_end - target_instance_id_end - 1);
        }

        std::string note_str;
        if(note_end == std::string::npos)
        {
            note_str = line.substr(event_end + 1);
        }
        else
        {
            note_str = line.substr(event_end +1, note_end - event_end -1);
        }

        uint64_t time{0};
        uint64_t tid{0};
        try
        {
            time = std::stoull(time_str);
            tid = std::stoull(tid_str);
        }
        catch (const std::exception& e)
        {
            printWarning() << "Could not prase time or instance id of line: " << line << " : " << e.what() << '\n';
            continue;
        }

        auto type = stringToEntityType(type_str);
        if (type == EntityTypes::unknown)
        {
            printWarning() << "could not parse type: " << type_str << "\n";
            continue;
        }

        if (is_waiting_for_full_migration_event)
        {
            if (type != btf::EntityTypes::task && type != btf::EntityTypes::isr)
            {
                printWarning() << " [time: " << time << "] a enforced_migration event must be followed by a task or isr event\n";
                is_waiting_for_full_migration_event = false;
            }
        }

        ErrorCodes err = ErrorCodes::success;
        switch (type)
        {
        case btf::EntityTypes::core: {
            auto c_ev = Core::stringToEvent(event_str);
            if (c_ev == Core::Events::unknown)
            {
                printWarning() << "could not parse core event (\"" << event_str << "\") of line: " << line << "\n";
                continue;
            }
            err = coreEvent(time, target, c_ev);
            break;
        }
        case btf::EntityTypes::os: {
            auto o_ev = OS::stringToEvent(event_str);
            if(o_ev == OS::Events::unknown)
            {
                printWarning() << "could not parse OS event (\"" << event_str << "\") of line: " << line << "\n";
                continue;
            }
            err = osEvent(time, source, target, o_ev);
            break;
        }
        case btf::EntityTypes::task:{
            auto t_ev = Process::stringToEvent(event_str);
            if (t_ev == Process::Events::unknown)
            {
                printWarning() << "could not parse task event (\"" << event_str << "\") of line: " << line << "\n";
                continue;
            }

            // handle migration: only enforced_migration with immediately followed full_migration is allowed
            if (t_ev == btf::Process::Events::enforced_migration)
            {
                // the next event must be a full_migration
                is_waiting_for_full_migration_event = true;
                migration_source_core = source;
                migration_task = target;
            }
            else if (t_ev == btf::Process::Events::full_migration)
            {
                if (is_waiting_for_full_migration_event)
                {
                    if (target == migration_task)
                    {
                        err = taskMigrationEvent(time, migration_source_core, source, target, tid);
                    }
                    else
                    {
                        printWarning() << " [time: " << time << "] got a full_migration event, but the previous enforced_migration had a different task\n";
                    }
                    is_waiting_for_full_migration_event = false;
                }
                else
                {
                    printWarning() << " [time: " << time << "] got a full_migration event, but the previous event was not a enforced_migration\n";
                }
            }
            else
            {
                if (is_waiting_for_full_migration_event)
                {
                    printWarning() << " [time: " << time << "] was waiting for a full_migration event, but got a different event\n";
                }
                // normal events
                err = processEvent(time, source, target, tid, t_ev);
                
            }
            break;
        }
        case btf::EntityTypes::isr:{
            auto isr_rev = Process::stringToEvent(event_str);
            if(isr_rev == Process::Events::unknown)
            {
                printWarning() << "could not parse isr event (\"" << event_str << "\") of line: " << line << "\n";
                continue;
            }
            err = processEvent(time, source, target, tid, isr_rev, true);
            break;
        }
        case btf::EntityTypes::stimulus: {
            auto sti_rev = Stimulus::stringToEvent(event_str);
            if(sti_rev == Stimulus::Events::unknown)
            {
                printWarning() << "could not parse stimulus event (\"" << event_str << "\") of line: " << line << "\n";
                continue;
            }
            err = stimulusEvent(time, source, target, sti_rev);
            break;
        }
        case btf::EntityTypes::semaphore: {
            auto sem_rev = Semaphore::stringToEvent(event_str);
            if(sem_rev == Semaphore::Events::unknown)
            {
                printWarning() << "could not parse semaphore event (\"" << event_str << "\") of line: " << line << "\n";
                continue;
            }
                err = semaphoreEvent(time, source, target, sem_rev, std::stoull(note_str));
            break;
        }
        case btf::EntityTypes::scheduler: {
            auto sched_rev = Scheduler::stringToEvent(event_str);
            if(sched_rev == Scheduler::Events::unknown)
            {
                printWarning() << "could not parse scheduler event (\"" << event_str << "\") of line: " << line << "\n";
                continue;
            }
                err = schedulerEvent(time, source, target, sched_rev);                  
            break;
        }
        case btf::EntityTypes::runnable: {
            auto r_ev = Runnable::stringToEvent(event_str);
            if (r_ev == Runnable::Events::unknown)
            {
                printWarning() << "could not parse runnable event (\"" << event_str << "\") of line: " << line << "\n";
                continue;
            }
            err = runnableEvent(time, source, target, r_ev);

            break;
        }
        case btf::EntityTypes::signal: {
            auto s_ev = Signal::stringToEvent(event_str);
            if (s_ev == Signal::Events::unknown)
            {
                printWarning() << "could not parse signal event (" << event_str << ") of line: " << line << "\n";
                continue;
            }
            std::string signal_value;
            if (s_ev == Signal::Events::write)
            {
                if (event_end != std::string::npos)
                {
                    signal_value = line.substr(event_end + 1);
                }
            }
            err = signalEvent(time, source, target, s_ev, signal_value);
            break;
        }
        default:
            FATAL_INTERNAL_ERROR_MSG("unknown type");
            break;
        }

        if (err != ErrorCodes::success)
        {
            printWarning() << "Could not emit event of line: " << line << " : " << errorCodeToString(err) << '\n';
        }
    }

    // enable auto generating events again
    auto_generate_events_ = true;
}

void BtfFile::finish()
{
    std::ofstream out(path_);
    out << getHeader();
    for (const auto& e : btf_entries_)
    {           
        out << e.toString(hash_map_) << '\n';           
    }

    // clear data that requires much memory
    cores_.clear();
    tasks_.clear();
    semaphores_.clear();
    runnables_.clear();
    runnable_stacks_.clear();
    task_core_map_.clear();
    os_iswait_.clear();
    hash_map_.clear();
    btf_entries_.clear();
    btf_entries_per_entity_.clear();
    type_map_.clear();
    current_running_tasks_.clear();
    did_de_allocated_task_event_occurred_on_core_.clear();
    stimuli_instance_ids_map_.clear();
    runnable_without_task_buffers_.clear();
    runnable_without_task_stacks_.clear();
    runnable_instance_id_counters_.clear();
    custom_header_entries_.clear();
}

void BtfFile::setStringHashMap(std::unordered_map<size_t, std::string> hash_map)
{
    hash_map_ = std::move(hash_map);
}

ErrorCodes BtfFile::coreEvent(uint64_t time, const std::string& core, Core::Events core_event)
{
    printTrace() << time << "," << core << "," << Core::eventToString(core_event) << "\n";

    size_t core_hash = std::hash<std::string>{}(core);
    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    hash_map_[core_hash] = core;
    return coreEvent(time, core_hash, core_event);
}

ErrorCodes BtfFile::coreEvent(uint64_t time, size_t core_hash, Core::Events core_event)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    // check: if core goes to idle no task must run on it
    if (core_event == Core::Events::idle && current_running_tasks_[core_hash] != no_running_task_)
    {
        return ErrorCodes::core_idle_task_still_running;
    }

    // check state transition
    er = cores_[core_hash].doStateTransition(core_event);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    // emit event
    btf_entries_.push_back({time, EntityTypes::core, core_hash, 0, core_hash, 0, core_event, ""});
    btf_entries_per_entity_[core_hash].push_back((--btf_entries_.end()));

    return er;
}

ErrorCodes BtfFile::osEvent(uint64_t time, const std::string& source, const std::string& os, OS::Events os_event)
{
    printTrace() << time << "," << source << "," << os << "," << OS::eventToString(os_event) << "\n";
    size_t core_hash;
    if(source_is_core_)
    {
        core_hash = std::hash<std::string>{}(source);
    }
    else
    {
        core_hash = std::hash<std::string>{}(task_core_map_[source]);
    }
    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    size_t os_hash = std::hash<std::string>{}(os);
    // we must do here a type check, otherwise we mess up our hash map
    er = checkType(os_hash, EntityTypes::os);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    if(source_is_core_)
    {
        hash_map_[core_hash] = source;
    }
    else
    {
        hash_map_[core_hash] = task_core_map_[source];
    }
    hash_map_[os_hash] = os;
    return osEvent(time, core_hash, os_hash, os_event);
}

ErrorCodes BtfFile::osEvent(uint64_t time, size_t core_hash, size_t os_hash, OS::Events os_event)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(os_hash, EntityTypes::os);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    auto task_id = current_running_tasks_[core_hash];
    //check if a task is running on the core
    if (task_id == no_running_task_)
    {
        return ErrorCodes::no_task_running;
    }

    // emit event
    btf_entries_.push_back({time, EntityTypes::os, task_id.first, task_id.second, os_hash, 0, BtfEntry::Events{os_event}, ""});
    btf_entries_per_entity_[os_hash].push_back((--btf_entries_.end()));

    if(auto_wait_resume_os_events_)
    {
        //check if OS-Event is wait or set. This is important because of task constraints:
        // - if wait_event: task needs to go to wait status.
        // - if set_event: if task was waiting for that event it needs to be released and resumed.
        const auto os_quadruple = std::make_pair(std::make_tuple(task_id.first, task_id.second, core_hash), os_hash);
        if(OS::eventToString(os_event) == "wait_event")
        {
            os_iswait_[os_quadruple] = true;
            processEvent(time, core_hash, task_id.first, task_id.second, Process::Events::wait);
            tasks_[task_id].setwaitOSevent(true);
        }
        else if(OS::eventToString(os_event) == "set_event")
        {    
            for(auto& count: os_iswait_)
            {
                if(count.second)
                {
                    if(count.first.second == os_hash)
                    {
                        processEvent(time, std::get<2>(count.first.first), std::get<0>(count.first.first), std::get<1>(count.first.first), Process::Events::release);
                        processEvent(time, std::get<2>(count.first.first), std::get<0>(count.first.first), std::get<1>(count.first.first), Process::Events::resume);
                        tasks_[task_id].setwaitOSevent(false);
                    }
                }
            }
        }
    }

    return er;
}

ErrorCodes BtfFile::taskMigrationEvent(uint64_t time, const std::string& source_core, const std::string& destination_core, const std::string& task,
                                       uint64_t task_instance_id)
{
    printTrace() << time << "," << task << "," << task_instance_id << " from " << source_core << " to " << destination_core << "\n";

    size_t source_core_hash = std::hash<std::string>{}(source_core);
    size_t destination_core_hash = std::hash<std::string>{}(destination_core);
    size_t task_hash = std::hash<std::string>{}(task);

    // check types
    ErrorCodes er = checkType(source_core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(destination_core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(task_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    hash_map_[source_core_hash] = source_core;
    hash_map_[destination_core_hash] = destination_core;
    hash_map_[task_hash] = task;
    return taskMigrationEvent(time, source_core_hash, destination_core_hash, task_hash, task_instance_id);
}

ErrorCodes BtfFile::taskMigrationEvent(uint64_t time, size_t source_core_hash, size_t destination_core_hash, size_t task_hash, uint64_t task_instance_id)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(source_core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(destination_core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(task_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    // check: the task must be not allocated to a core
    auto task_id = std::make_pair(task_hash, task_instance_id);
    for (auto [c, t] : current_running_tasks_)
    {
        if (t == task_id)
        {
            return ErrorCodes::invalid_state_transition;
        }
    }

    // state transition is always possible except in terminated
    if (tasks_[task_id].isTerminated())
    {
        return ErrorCodes::invalid_state_transition;
    }

    // emit events
    btf_entries_.push_back(
        {time, EntityTypes::task, source_core_hash, 0, task_id.first, task_id.second, BtfEntry::Events{Process::Events::enforced_migration}, ""});
    btf_entries_per_entity_[task_id.first].push_back((--btf_entries_.end()));
    btf_entries_.push_back(
        {time, EntityTypes::task, destination_core_hash, 0, task_id.first, task_id.second, BtfEntry::Events{Process::Events::full_migration}, ""});
    btf_entries_per_entity_[task_id.first].push_back((--btf_entries_.end()));

    return ErrorCodes::success;
}

ErrorCodes BtfFile::processEvent(uint64_t time, const std::string& source, const std::string& process, uint64_t process_instance_id, Process::Events process_event,
                              bool is_isr)
{
    printTrace() << time << "," << source << "," << process << "," << process_instance_id << "," << Process::eventToString(process_event) << "\n";

    size_t source_hash = std::hash<std::string>{}(source);
    size_t process_hash = std::hash<std::string>{}(process);

    // check types
    ErrorCodes er = checkType(source_hash, Process::getSourceType(process_event));
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(process_hash, is_isr ? EntityTypes::isr : EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    //add source and task to the hash map.
    hash_map_[source_hash] = source;
    hash_map_[process_hash] = process;
    return processEvent(time, source_hash, process_hash, process_instance_id, process_event, is_isr);
}

ErrorCodes BtfFile::processEvent(uint64_t time, size_t source_hash, size_t process_hash, uint64_t process_instance_id, Process::Events process_event, bool is_isr)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(process_hash, is_isr ? EntityTypes::isr : EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(source_hash, Process::getSourceType(process_event));
    if (er != ErrorCodes::success)
    {
        return er;
    }

    auto task_id = std::make_pair(process_hash, process_instance_id);

    if (Process::getSourceType(process_event) == EntityTypes::core)
    {
        // check if core is idle (only if no auto generation)
        if (!auto_generate_core_events_ && cores_[source_hash].isIdle())
        {
            return ErrorCodes::event_on_idle_core;
        }

        // check if another task is running
        if (current_running_tasks_[source_hash] != no_running_task_ && Process::isEventAllocatingCore(process_event))
        {
            // if the task of this event is running --> invalid state transition
            if (task_id == current_running_tasks_[source_hash])
            {
                return ErrorCodes::invalid_state_transition;
            }
            return ErrorCodes::multiple_tasks_running;
        }

        // if the task is deallocating the core it must be running on it
        // do not check this if this is the very first event on a core
        if (did_task_allocation_event_happen_on_core_[source_hash])
        {
            if (Process::isEventDeallocatingCore(process_event))
            {
                if (task_id != current_running_tasks_[source_hash])
                {
                    return ErrorCodes::invalid_state_transition;
                }
            }
        }

        if (Process::isEventAllocatingCore(process_event))
        {
            did_task_allocation_event_happen_on_core_[source_hash] = true;
        }
    }

    // check if this task is currently allocated to another core
    for (const auto& [s, t] : current_running_tasks_)
    {
        if (s != source_hash && task_id == t)
        {
            return ErrorCodes::allocated_to_different_core;
        }
    }

    // if terminate -> check if there are still runnables
    if (process_event == Process::Events::terminate && !runnable_stacks_[task_id].empty())
    {
        return ErrorCodes::terminate_on_task_with_running_runnables;
    }

    er = tasks_[task_id].doStateTransition(process_event);
    if (er == ErrorCodes::success)
    {
        if (auto_generate_core_events_)
        {
            generateCoreExecuteEvent(time, source_hash, process_event);
        }

        // if deallocating core -> suspend all runnables, which are running
        if (Process::isEventDeallocatingCore(process_event))
        {
            for (auto it = runnable_stacks_[task_id].rbegin(); it != runnable_stacks_[task_id].rend(); ++it)
            {
                const auto r_id = *it;
                if (runnables_[r_id].isRunning())
                {
                    if (runnableEvent(time, source_hash, process_hash, r_id.first, Runnable::Events::suspend) != ErrorCodes::success)
                    {
                        FATAL_INTERNAL_ERROR_MSG("could not suspend runnable");
                    }
                    runnables_[r_id].setWasSuspendedByTaskPreempt(true);
                }
            }
        }

        size_t source_id{0};
        if (Process::getSourceType(process_event) == EntityTypes::stimulus)
        {
            source_id = stimuli_instance_ids_map_[source_hash];
        }

        btf_entries_.push_back(
            {time, is_isr ? EntityTypes::isr : EntityTypes::task, source_hash, source_id, process_hash, process_instance_id, BtfEntry::Events{process_event}, ""});
        btf_entries_per_entity_[process_hash].push_back((--btf_entries_.end()));

        bool was_first_de_alloc{false};
        if(source_is_core_)
        {
            if (!did_de_allocated_task_event_occurred_on_core_[source_hash])
            {
                // first allocating task event on a core -> poll and run indicates a already running task
                if (process_event == Process::Events::poll || process_event == Process::Events::run || Process::isEventAllocatingCore(process_event))
                {
                    did_de_allocated_task_event_occurred_on_core_[source_hash] = true;
                    was_first_de_alloc = true;
                    current_running_tasks_[source_hash] = task_id;
                    // map the core to the process
                    task_core_map_[hash_map_[process_hash]] = hash_map_[source_hash];
                }
            }
            else
            {
                if (Process::isEventAllocatingCore(process_event))
                {
                    current_running_tasks_[source_hash] = task_id;
                    // map the core to the process
                    task_core_map_[hash_map_[process_hash]] = hash_map_[source_hash];
                }
            }
        }
        else
        {
            if (!did_de_allocated_task_event_occurred_on_core_[process_hash])
            {
                // first allocating task event on a core -> poll and run indicates a already running task
                if (process_event == Process::Events::poll || process_event == Process::Events::run || Process::isEventAllocatingCore(process_event))
                {
                    did_de_allocated_task_event_occurred_on_core_[process_hash] = true;
                    was_first_de_alloc = true;
                    current_running_tasks_[source_hash] = task_id;
                    // map the core to the process
                    task_core_map_[hash_map_[process_hash]] = hash_map_[source_hash];
                }
            }
            else
            {
                if (Process::isEventAllocatingCore(process_event))
                {
                    current_running_tasks_[source_hash] = task_id;
                    // map the core to the process
                    task_core_map_[hash_map_[process_hash]] = hash_map_[source_hash];
                }
            }
        }
        

        if (Process::isEventDeallocatingCore(process_event))
        {
            current_running_tasks_[source_hash] = no_running_task_;
            if(source_is_core_)
            {
                if (!did_de_allocated_task_event_occurred_on_core_[process_hash])
                {
                    did_de_allocated_task_event_occurred_on_core_[process_hash] = true;
                    was_first_de_alloc = true;
                }
            }
            else
            {
                if (!did_de_allocated_task_event_occurred_on_core_[source_hash])
                {
                    did_de_allocated_task_event_occurred_on_core_[source_hash] = true;
                    was_first_de_alloc = true;
                }
            }
            

            if (auto_generate_core_events_)
            {
                generateCoreIdleEvent(time, source_hash);
            }
        }

        // if the task did his first (De)allocation we must look if we have "loose" runnables
        if (was_first_de_alloc)
        {
            if(source_is_core_)
            {
                for (auto& e : runnable_without_task_buffers_[source_hash])
                {
                    e->source_hash_ = process_hash;
                    e->source_instance_ = process_instance_id;
                }
                runnable_without_task_buffers_[source_hash].clear();
    
                if (!runnable_without_task_stacks_[source_hash].empty())
                {
                    runnable_stacks_[task_id] = runnable_without_task_stacks_[source_hash];
                }
            }
            else
            {
                for (auto& e : runnable_without_task_buffers_[process_hash])
                {
                    e->source_hash_ = process_hash;
                    e->source_instance_ = process_instance_id;
                }
                runnable_without_task_buffers_[process_hash].clear();

                if (!runnable_without_task_stacks_[process_hash].empty())
                {
                    runnable_stacks_[task_id] = runnable_without_task_stacks_[process_hash];
                }
            }
        }

        // if allocating core -> resume all runnables
        if (Process::isEventAllocatingCore(process_event))
        {
            for (const auto& runnable : runnable_stacks_[task_id])
            {
                const auto r_hash = runnable.first;
                if (runnables_[runnable].wasSuspendedByTaskPreempt())
                {
                    if (runnableEvent(time, source_hash, process_hash, r_hash, Runnable::Events::resume) != ErrorCodes::success)
                    {
                        FATAL_INTERNAL_ERROR_MSG("could not resume runnable");
                    }
                    runnables_[runnable].setWasSuspendedByTaskPreempt(false);
                }
            }
        }

        // if terminated remove this task from the runnable stacks
        if (process_event == Process::Events::terminate)
        {
            runnable_stacks_.erase(task_id);
        }
    }

    // ignore multiple releases
    if (ignore_multiple_task_releases_)
    {
        if (er == ErrorCodes::already_in_state && process_event == Process::Events::release)
        {
            er = ErrorCodes::success;
        }
    }


    return er;
}

ErrorCodes BtfFile::runnableEvent(uint64_t time, const std::string& source, const std::string& runnable, Runnable::Events runnable_event)
{
    printTrace() << time << "," << source << "," << runnable << "," << Runnable::eventToString(runnable_event) << "\n";

    size_t core_hash;
    size_t process_hash;
    size_t runnable_hash = std::hash<std::string>{}(runnable);
    if(source_is_core_)
    {
        core_hash = std::hash<std::string>{}(source);
        process_hash = std::hash<std::string>{}(hash_map_[current_running_tasks_[core_hash].first]);
    }
    else
    {
        core_hash = std::hash<std::string>{}(task_core_map_[source]);
        process_hash = std::hash<std::string>{}(source);
    }
    

    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(runnable_hash, EntityTypes::runnable);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    if(source_is_core_)
    {
        hash_map_[core_hash] = task_core_map_[source];
    }
    else
    {
        hash_map_[core_hash] = source;
    }
    
    hash_map_[runnable_hash] = runnable;
    return runnableEvent(time, core_hash, process_hash, runnable_hash, runnable_event);
}

ErrorCodes BtfFile::runnableEvent(uint64_t time, size_t core_hash, size_t process_hash, size_t runnable_hash, Runnable::Events runnable_event)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(runnable_hash, EntityTypes::runnable);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    // if we do not know the task (e.g. runnable event is first event) or if we do not have a start event on the source
    // task we can never know how many runnables were still running before the trace started so if we get a runnable
    // event:
    //        - start: no special action necessary
    //        - terminate: the runnable_stack can be empty
    //        - suspend: add to stack
    //        - resume: add to stack

    // what if runnable event is the very first event
    // in case no task event occurred on this core, we can not check for running task
    // we can also not emit BTF events since the task is the source of a runnable event
    // so we must save iterators to the emitted events and change the task at the moment the first task event occurred
    // Problem: if never a task event occurs we lose this events --> but this seems legit to ignore
    bool is_pre_task_event{false};
    std::pair<size_t, uint64_t> task_id;

    //first check, if the core/process even exists
    if(source_is_core_)
    {
        if (!did_de_allocated_task_event_occurred_on_core_[core_hash])
        {
            is_pre_task_event = true;
        }
        else
        {
            // get the current running task
            task_id = current_running_tasks_[core_hash];
            if (task_id == no_running_task_)
            {
                return ErrorCodes::no_task_running;
            }
        }
    }
    else
    {
        if (!did_de_allocated_task_event_occurred_on_core_[process_hash])
        {
            is_pre_task_event = true;
        }
        else
        {
            // get the current running task
            task_id = current_running_tasks_[core_hash];
            if (task_id == no_running_task_)
            {
                return ErrorCodes::no_task_running;
            }
        }       
    }
    
    // Lambda that gets the references to the runnable stack depending on if the source is a core or a process.
    auto& runnable_stack = [&]() -> auto & {
        if(source_is_core_)
        {
            return is_pre_task_event ? runnable_without_task_stacks_[core_hash] : runnable_stacks_[task_id];
        }
        else
        {
            return is_pre_task_event ? runnable_without_task_stacks_[process_hash] : runnable_stacks_[task_id];
        }
    }();

    // get or compute the instance id
    uint64_t runnable_instance_id{0};
    bool get_new_instance_id{false};
    ErrorCodes search_error{ErrorCodes::success};

    if (runnable_event == btf::Runnable::Events::start)
    {
        get_new_instance_id = true;
    }
    else
    {
        if (runnable_stack.empty())
        {
            search_error = ErrorCodes::runnable_source_task_not_running;
        }
        else
        {
            if (runnable_event == btf::Runnable::Events::terminate)
            {
                if (runnable_stack.back().first == runnable_hash)
                {
                    runnable_instance_id = runnable_stack.back().second;
                }
                else
                {
                    
                    auto p = std::find_if(runnable_stack.rbegin(), runnable_stack.rend(), [runnable_hash](auto& a) { return a.first == runnable_hash; });
                    if(p != runnable_stack.rend())
                    {
                      // terminate all sub runnables
                      auto runnable_stack_copy{runnable_stack};
                      for(size_t i=runnable_stack_copy.size(); i>0; --i)
                      {
                        if(runnable_stack_copy[i-1].first == runnable_hash)
                        {
                          runnable_instance_id = runnable_stack_copy[i-1].second;
                          break;
                        }
                        else
                        {
                          runnableEvent(time, core_hash, process_hash, runnable_stack_copy[i-1].first, btf::Runnable::Events::terminate);
                        }
                      }
                    }
                    else 
                    {
                      search_error = ErrorCodes::runnable_source_task_not_running;
                    }
                }
            }
            else
            {
                if (runnable_event == btf::Runnable::Events::resume)
                {
                    // if we auto suspend the parent runnable, only the top most runnable is allowed to resume
                    // otherwise only the lowest not running runnable is allowed to resume

                    if (auto_suspend_parent_runnable_)
                    {
                        if (runnable_stack.back().first == runnable_hash)
                        {
                            runnable_instance_id = runnable_stack.back().second;
                        }
                        else
                        {
                            search_error = ErrorCodes::runnable_source_task_not_running;
                        }
                    }
                    else
                    {
                        // is the lowest not running
                        for (const auto& runnable : runnable_stack)
                        {
                            if (!runnables_[runnable].isRunning())
                            {
                                if (runnable.first == runnable_hash)
                                {
                                    runnable_instance_id = runnable.second;
                                }
                                else
                                {
                                    search_error = ErrorCodes::runnable_source_task_not_running;
                                }
                                break;
                            }
                        }
                    }
                }
                else
                {
                    // is the top most running
                    for (auto it = runnable_stack.rbegin(); it != runnable_stack.rend(); ++it)
                    {
                        if (runnables_[*it].isRunning())
                        {
                            if (it->first == runnable_hash)
                            {
                                runnable_instance_id = it->second;
                            }
                            else
                            {
                                search_error = ErrorCodes::runnable_source_task_not_running;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    if (search_error != ErrorCodes::success)
    {
        if (is_pre_task_event)
        {
            get_new_instance_id = true;
        }
        else
        {
            return search_error;
        }
    }

    if (get_new_instance_id)
    {
        runnable_instance_id = runnable_instance_id_counters_[runnable_hash]++;
    }

    const auto runnable_id = std::make_pair(runnable_hash, runnable_instance_id);

    // if runnable is terminating it must be the last on the stack
    if (runnable_event == Runnable::Events::terminate)
    {
        if (!is_pre_task_event && tasks_[task_id].wasStarted())
        {
            if (runnable_stack.back() != runnable_id)
            {
                return ErrorCodes::terminate_on_runnable_with_running_sub_runnable;
            }
        }
        else
        {
            if (!runnable_stack.empty())
            {
                if (runnable_stack.back() != runnable_id)
                {
                    return ErrorCodes::terminate_on_runnable_with_running_sub_runnable;
                }
            }
        }
    }

    er = runnables_[runnable_id].doStateTransition(runnable_event);
    if (er == ErrorCodes::success)
    {
        if (runnable_event == Runnable::Events::start)
        {
            // add to stack, suspend previous runnables
            // check if it is already in the stack
            if (std::find(runnable_stack.begin(), runnable_stack.end(), runnable_id) != runnable_stack.end())
            {
                FATAL_INTERNAL_ERROR_MSG("starting runnable already in runnable stack");
            }

            if (auto_generate_events_ && auto_suspend_parent_runnable_ && !runnable_stack.empty())
            {
                auto parent_runnable_id = runnable_stack.back();
                if (runnables_[parent_runnable_id].isRunning())
                {
                    if (runnableEvent(time, core_hash, process_hash, parent_runnable_id.first, Runnable::Events::suspend) != ErrorCodes::success) // NOLINT
                    {
                        FATAL_INTERNAL_ERROR_MSG("error while generation runnable suspend");
                    }
                }
            }
            runnable_stack.push_back(runnable_id);
        }

        if (runnable_event == Runnable::Events::resume || runnable_event == Runnable::Events::suspend)
        {
            bool add_to_stack{false};
            if (is_pre_task_event || !tasks_[task_id].wasStarted())
            {
                add_to_stack = true;
            }
            if (add_to_stack)
            {
                if (std::find(runnable_stack.begin(), runnable_stack.end(), runnable_id) == runnable_stack.end())
                {
                    runnable_stack.push_back(runnable_id);
                }
            }
        }

        btf_entries_.push_back(
            {time, EntityTypes::runnable, task_id.first, task_id.second, runnable_hash, runnable_instance_id, BtfEntry::Events{runnable_event}, ""});
        btf_entries_per_entity_[runnable_hash].push_back((--btf_entries_.end()));
        if (is_pre_task_event)
        {
            if(source_is_core_)
            {
                runnable_without_task_buffers_[core_hash].push_back((--btf_entries_.end()));
            }
            else
            {
                runnable_without_task_buffers_[process_hash].push_back((--btf_entries_.end()));
            }

        }

        if (runnable_event == Runnable::Events::terminate)
        {
            // if we do not know the task or it did not started, we do not need to check here
            if (!is_pre_task_event && tasks_[task_id].wasStarted())
            {
                // remove from stack, resume previous runnable
                if (runnable_stack.empty())
                {
                    FATAL_INTERNAL_ERROR_MSG("empty runnable stack on terminate");
                }
                if (runnable_stack.back() != runnable_id)
                {
                    FATAL_INTERNAL_ERROR_MSG("terminate on runnable with sub runnables");
                }
                runnable_stack.pop_back();
            }
            else
            {
                if (!runnable_stack.empty())
                {
                    if (runnable_stack.back() != runnable_id)
                    {
                        FATAL_INTERNAL_ERROR_MSG("terminate on runnable with sub runnables");
                    }
                    runnable_stack.pop_back();
                }
            }

            if (auto_generate_events_ && auto_suspend_parent_runnable_ && !runnable_stack.empty())
            {
                auto previous_runnable_id = runnable_stack.back();
                if (runnables_[previous_runnable_id].isRunning())
                {
                    FATAL_INTERNAL_ERROR_MSG("previous runnable still running");
                }
                if (runnableEvent(time, core_hash, process_hash, previous_runnable_id.first, Runnable::Events::resume) != ErrorCodes::success)
                {
                    FATAL_INTERNAL_ERROR_MSG("error while generation runnable resume");
                }
            }
        }
    }
    return er;
}

ErrorCodes BtfFile::schedulerEvent(uint64_t time, const std::string& source, const std::string& scheduler, Scheduler::Events scheduler_event)
{
    printTrace() << time << "," << source << "," << scheduler << "," << Scheduler::eventToString(scheduler_event) << "\n";

    size_t source_hash = std::hash<std::string>{}(source);
    size_t scheduler_hash = std::hash<std::string>{}(scheduler);
    ErrorCodes er;

    // check if it is a schedule event or a schedulepoint event
    if(Scheduler::eventToString(scheduler_event)=="schedule")
    {
        // we must do here a type check, otherwise we mess up our hash map
        er = checkType(source_hash, EntityTypes::scheduler);
        if (er != ErrorCodes::success)
        {   
            return er;   
        }
        er = checkType(scheduler_hash, EntityTypes::scheduler);
        if (er != ErrorCodes::success)
        {
            return er;
        }
        hash_map_[scheduler_hash] = scheduler;
        return schedulerEvent(time, scheduler_hash, scheduler_event);
    }
    else if(Scheduler::eventToString(scheduler_event)=="schedulepoint")
    {   
         size_t core_hash;
        // we must do here a type check, otherwise we mess up our hash map
        if(source_is_core_)
        {
            er = checkType(source_hash, EntityTypes::core);
            core_hash = std::hash<std::string>{}(source);
        }
        else
        {
            er = checkType(source_hash, EntityTypes::task);
            //here we must get the core from the source which is a task!
            core_hash = std::hash<std::string>{}(task_core_map_[source]);
        }
        if (er != ErrorCodes::success)
        {   
            return er;   
        }
        er = checkType(scheduler_hash, EntityTypes::scheduler);
        if (er != ErrorCodes::success)
        {
            return er;
        }

        
        er = checkType(core_hash, EntityTypes::core);
        if (er != ErrorCodes::success)
        {   
            return er;   
        }
        if(source_is_core_)
        {
            hash_map_[core_hash] = source;
        }
        else
        {
            hash_map_[core_hash] = task_core_map_[source];
        }
        hash_map_[scheduler_hash] = scheduler;
        
        return schedulerEvent(time, core_hash, scheduler_hash, scheduler_event);
    }
    else
    {
        return ErrorCodes::invalid_event;
    }
    
}

ErrorCodes BtfFile::schedulerEvent(uint64_t time, size_t scheduler_hash, Scheduler::Events scheduler_event)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(scheduler_hash, EntityTypes::scheduler);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    

    btf_entries_.push_back({time, EntityTypes::scheduler, scheduler_hash, 0, scheduler_hash, 0, BtfEntry::Events{scheduler_event}, ""});
    btf_entries_per_entity_[scheduler_hash].push_back((--btf_entries_.end()));


    return ErrorCodes::success;
}

ErrorCodes BtfFile::schedulerEvent(uint64_t time, size_t core_hash, size_t scheduler_hash, Scheduler::Events scheduler_event)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        er = ErrorCodes::terminate_on_runnable_with_running_sub_runnable;
        return er;   
    }
    er = checkType(scheduler_hash, EntityTypes::scheduler);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    
    auto task_id = current_running_tasks_[core_hash];
    if (task_id == no_running_task_)
    {
        return ErrorCodes::no_task_running;
    }

    btf_entries_.push_back({time, EntityTypes::scheduler, task_id.first, task_id.second, scheduler_hash, 0, BtfEntry::Events{scheduler_event}, ""});
    btf_entries_per_entity_[scheduler_hash].push_back((--btf_entries_.end()));


    return ErrorCodes::success;
}

ErrorCodes BtfFile::semaphoreEvent(uint64_t time, const std::string& source, const std::string& target, Semaphore::Events semaphore_event, uint64_t note)
{
    printTrace() << time << "," << source << "," << target << "," << "," << Semaphore::eventToString(semaphore_event) << "\n";

    size_t source_hash = std::hash<std::string>{}(source);
    size_t target_hash = std::hash<std::string>{}(target);
    ErrorCodes er = ErrorCodes::success;

    //this event check is necessary to determine which source type to expect.
    switch(semaphore_event)
    {
        case Semaphore::Events::free:
        case Semaphore::Events::unlock:
        case Semaphore::Events::lock:
        case Semaphore::Events::unlock_full:
        case Semaphore::Events::used:
        case Semaphore::Events::full:
        case Semaphore::Events::lock_used:
        case Semaphore::Events::overfull:
            // check types
            er = checkType(source_hash, EntityTypes::semaphore);
            if (er != ErrorCodes::success)
            {
                return er;
            }
            er = checkType(target_hash, EntityTypes::semaphore);
            if (er != ErrorCodes::success)
            {
                return er;
            }
            if(source != target)
            {
                return ErrorCodes::source_and_target_not_equal;
            }

            hash_map_[target_hash] = target;
            return semaphoreEvent(time, target_hash, semaphore_event, note); 
            break;
        case Semaphore::Events::decrement:
        case Semaphore::Events::increment:
        case Semaphore::Events::released:
        case Semaphore::Events::requestsemaphore:
        case Semaphore::Events::assigned:
        case Semaphore::Events::queued:
        case Semaphore::Events::waiting:
            if(source_is_core_)
            {
                // check types before adding to hash_map
                er = checkType(source_hash, EntityTypes::core);
                if (er != ErrorCodes::success)
                {
                        return er;
                } 
                er = checkType(target_hash, EntityTypes::semaphore);
                if (er != ErrorCodes::success)
                {
                    return er;
                }             
                hash_map_[source_hash] = source;
                hash_map_[target_hash] = target;
                return semaphoreEvent(time, source_hash, target_hash, semaphore_event, note);
            }
            else
            {
                size_t core_hash = std::hash<std::string>{}(task_core_map_[source]);

                // check types before adding to hash_map
                er = checkType(source_hash, EntityTypes::task);
                if (er != ErrorCodes::success)
                {
                    er = checkType(source_hash, EntityTypes::isr);
                    if(er != ErrorCodes::success)
                    {
                        return er;
                    }
                }
                er = checkType(target_hash, EntityTypes::semaphore);
                if (er != ErrorCodes::success)
                {
                    return er;
                }              
                er = checkType(core_hash, EntityTypes::core);
                if (er != ErrorCodes::success)
                {
                    return er;
                }

                hash_map_[core_hash] = task_core_map_[source];
                hash_map_[target_hash] = target;
                return semaphoreEvent(time, core_hash, target_hash, semaphore_event, note);
            }
            break;
        default:
            return ErrorCodes::invalid_event;
            break;
    }
}

ErrorCodes BtfFile::semaphoreEvent(uint64_t time, size_t semaphore_hash, Semaphore::Events semaphore_event, uint64_t note)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(semaphore_hash, EntityTypes::semaphore);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    
    switch(semaphore_event)
    {
        case Semaphore::Events::free:
        case Semaphore::Events::unlock:
            if(note != 0) return ErrorCodes::amount_of_semaphore_accesses_invalid;           
            break;
        case Semaphore::Events::lock:
            if(note != 1) return ErrorCodes::amount_of_semaphore_accesses_invalid;
            break;
        case Semaphore::Events::unlock_full:
        case Semaphore::Events::used:
            if(note <= 0) return ErrorCodes::amount_of_semaphore_accesses_invalid;
            break;
        case Semaphore::Events::full:
        case Semaphore::Events::lock_used:
        case Semaphore::Events::overfull:
            //do nothing
            break;
        default:
            return ErrorCodes::invalid_event;
    }
    semaphores_[semaphore_hash].doStateTransition(semaphore_event);

    btf_entries_.push_back({time, EntityTypes::semaphore, semaphore_hash, 0, semaphore_hash, 0, BtfEntry::Events{semaphore_event}, ""});
    btf_entries_per_entity_[semaphore_hash].push_back((--btf_entries_.end()));

    //add the note
    btf_entries_.back().note_ = std::to_string(note);

    return ErrorCodes::success;
}

ErrorCodes BtfFile::semaphoreEvent(uint64_t time, size_t core_hash, size_t semaphore_hash, Semaphore::Events semaphore_event, uint64_t note)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(semaphore_hash, EntityTypes::semaphore);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    auto task_id = current_running_tasks_[core_hash];
    
    switch(semaphore_event)
    {
        
        case Semaphore::Events::decrement:
        case Semaphore::Events::increment:
        case Semaphore::Events::released:
        case Semaphore::Events::requestsemaphore:
            if (task_id == no_running_task_) return ErrorCodes::no_task_running;
            break;
        case Semaphore::Events::assigned:
        case Semaphore::Events::queued:
        case Semaphore::Events::waiting:
            //do nothing
            break;
        default:
            return ErrorCodes::invalid_event;
    }
    semaphores_[semaphore_hash].doStateTransition(semaphore_event);
    if(note==1){}
    btf_entries_.push_back({time, EntityTypes::semaphore, task_id.first, task_id.second, semaphore_hash, 0, BtfEntry::Events{semaphore_event}, ""});
    btf_entries_per_entity_[semaphore_hash].push_back((--btf_entries_.end()));
    
    //add the note
    btf_entries_.back().note_ = std::to_string(note);

    return ErrorCodes::success;
}

ErrorCodes BtfFile::signalEvent(uint64_t time, const std::string& source, const std::string& signal, Signal::Events signal_event, const std::string& signal_value)
{
    printTrace() << time << "," << source << "," << signal << "," << Signal::eventToString(signal_event) << "," << signal_value << "\n";

    size_t core_hash;
    size_t signal_hash = std::hash<std::string>{}(signal);

    if(source_is_core_)
    {
        core_hash = std::hash<std::string>{}(source);
    }
    else
    {
        core_hash = std::hash<std::string>{}(task_core_map_[source]);
    }

    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(signal_hash, EntityTypes::signal);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    if(source_is_core_)
    {
    hash_map_[core_hash] = source;
    }
    else
    {
    hash_map_[core_hash] = task_core_map_[source];
    }
    hash_map_[signal_hash] = signal;
    return signalEvent(time, core_hash, signal_hash, signal_event, signal_value);
}

ErrorCodes BtfFile::signalEvent(uint64_t time, size_t core_hash, size_t signal_hash, Signal::Events signal_event, const std::string& signal_value)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(signal_hash, EntityTypes::signal);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(core_hash, EntityTypes::core);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    auto task_id = current_running_tasks_[core_hash];
    if (task_id == no_running_task_)
    {
        return ErrorCodes::no_task_running;
    }

    btf_entries_.push_back({time, EntityTypes::signal, task_id.first, task_id.second, signal_hash, 0, BtfEntry::Events{signal_event}, ""});
    btf_entries_per_entity_[signal_hash].push_back((--btf_entries_.end()));

    if (!signal_value.empty())
    {
        // remove all newlines
        std::string processed;
        for (const auto& c : signal_value)
        {
            if (c == '\n' || c == '\r')
            {
                continue;
            }
            processed.push_back(c);
        }
        btf_entries_.back().note_ = processed;
    }

    return ErrorCodes::success;
}

ErrorCodes BtfFile::stimulusEvent(uint64_t time, const std::string& source, const std::string& target, Stimulus::Events stimulus_event)
{
    printTrace() << time << "," << source << "," << target << "," << Stimulus::eventToString(stimulus_event) << "\n";

    size_t stimulus_hash = std::hash<std::string>{}(source);
    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(stimulus_hash, EntityTypes::stimulus);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    //check if source and target are the same.
    if(source != target)
    {
        return ErrorCodes::source_and_target_not_equal;
    }

    hash_map_[stimulus_hash] = source;
    return stimulusEvent(time, stimulus_hash, stimulus_event);
}

ErrorCodes BtfFile::stimulusEvent(uint64_t time, size_t stimulus_hash, Stimulus::Events stimulus_event)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(stimulus_hash, EntityTypes::stimulus);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    if(stimuli_instance_ids_map_.find(stimulus_hash) == stimuli_instance_ids_map_.end())
    {
        stimuli_instance_ids_map_[stimulus_hash]=0;
        btf_entries_.push_back({time, EntityTypes::stimulus, stimulus_hash, stimuli_instance_ids_map_[stimulus_hash], stimulus_hash, stimuli_instance_ids_map_[stimulus_hash], BtfEntry::Events{stimulus_event}, ""});
        btf_entries_per_entity_[stimulus_hash].push_back((--btf_entries_.end()));
    }
    else
    {
        stimuli_instance_ids_map_[stimulus_hash]= stimuli_instance_ids_map_[stimulus_hash]+1;
        btf_entries_.push_back({time, EntityTypes::stimulus, stimulus_hash, stimuli_instance_ids_map_[stimulus_hash], stimulus_hash, stimuli_instance_ids_map_[stimulus_hash], BtfEntry::Events{stimulus_event}, ""});
        btf_entries_per_entity_[stimulus_hash].push_back((--btf_entries_.end()));
    }


    return ErrorCodes::success;
}

void BtfFile::comment(const std::string& comment)
{
    // remove newlines
    std::string note = comment;
    if (!note.empty() && note.back() == '\n')
    {
        note = note.substr(0, note.size() - 1);
    }

    btf_entries_.push_back({0, EntityTypes::comment, 0, 0, 0, 0, BtfEntry::Events{Process::Events::unknown}, ""});
    btf_entries_.back().note_ = note;
}

void BtfFile::headerEntry(const std::string& header_entry)
{
    // remove newline
    std::string e = header_entry;
    if (!e.empty() && e.back() == '\n')
    {
        e = e.substr(0, e.size() - 1);
    }

    // remove leading hash-tag
    if (!e.empty() && e[0] == '#')
    {
        e = e.substr(1);
    }

    if (!e.empty())
    {
        custom_header_entries_.push_back(e);
    }
}

void BtfFile::insertEvent(std::list<std::list<BtfEntry>::iterator>::iterator pos, const BtfEntry& entry)
{
    auto new_it = btf_entries_.insert(*pos, entry);
    btf_entries_per_entity_[entry.source_hash_].insert(pos, new_it);
}

std::list<std::list<BtfEntry>::iterator>& BtfFile::getEntityEvents(const std::string& entity)
{
    size_t hash = std::hash<std::string>{}(entity);
    return btf_entries_per_entity_[hash];
}

std::list<std::list<BtfEntry>::iterator> BtfFile::getEventsForEntity(const std::string& entity)
{
    return getEventsForEntity(std::hash<std::string>{}(entity));
}

std::list<std::list<BtfEntry>::iterator> BtfFile::getEventsForEntity(size_t entity_hash)
{
    if (btf_entries_per_entity_.count(entity_hash) > 0)
    {
        return btf_entries_per_entity_[entity_hash];
    }
    return {};
}

void BtfFile::generateCoreIdleEvent(uint64_t time, size_t source_hash)
{
    if (coreEvent(time, source_hash, Core::Events::idle) != ErrorCodes::success)
    {
        FATAL_INTERNAL_ERROR_MSG("could not auto generate core event")
    }
}

void BtfFile::generateCoreExecuteEvent(uint64_t time, size_t source_hash, Process::Events process_event)
{
    // does task goes to allocated to core?
    if (Process::isEventAllocatingCore(process_event))
    {
        // check if last core event was a idle_execute
        if (btf_entries_per_entity_.count(source_hash) > 0 && !btf_entries_per_entity_[source_hash].empty() &&
            btf_entries_per_entity_[source_hash].back()->event_.core_event == Core::Events::execute)
        {
            // if the idle_execute happened at the same time like this event -> remove it
            if (btf_entries_per_entity_[source_hash].back()->time_ == time)
            {
                btf_entries_.erase(btf_entries_per_entity_[source_hash].back());
                btf_entries_per_entity_[source_hash].pop_back();
            }
        }
        else // emit execute
        {
            if (coreEvent(time, source_hash, Core::Events::execute) != ErrorCodes::success)
            {
                FATAL_INTERNAL_ERROR_MSG("could not auto generate core event")
            }
        }
    }
}

ErrorCodes BtfFile::checkTime(uint64_t time)
{
    if (last_time_ > time)
    {
        return ErrorCodes::descending_timestamp;
    }
    last_time_ = time;
    return ErrorCodes::success;
}

ErrorCodes BtfFile::checkType(size_t hash, EntityTypes should_be_type)
{
    if (type_map_.count(hash) > 0)
    {
        if (type_map_[hash] != should_be_type)
        {
            return ErrorCodes::invalid_type;
        }
    }
    else
    {
        type_map_[hash] = should_be_type;
    }
    return ErrorCodes::success;
}

std::string BtfFile::getHeader() const
{
    std::string ret("#version 2.2.1\n#creator libBtf\n#timescale ");
    switch (time_scale_)
    {
    case TimeScales::pico_seconds:
        ret.append("ps");
        break;
    case TimeScales::nano_seconds:
        ret.append("ns");
        break;
    case TimeScales::micro_seconds:
        ret.append("us");
        break;
    case TimeScales::milli_seconds:
        ret.append("ms");
        break;
    }
    ret.append("\n");

    for (const auto& e : custom_header_entries_)
    {
        ret.append("#");
        ret.append(e);
        ret.append("\n");
    }

    return ret;
}

std::list<BtfEntry> BtfFile::getAllEvents() const
{
    return btf_entries_;
}

size_t BtfFile::getNumberOfAllEvents() const
{
    return btf_entries_.size();
}

ErrorCodes BtfFile::simulationEventProcessName(uint64_t time, const std::string& process, const std::string& name)
{
    printTrace() << time << "," << process << ","
                                   << "ProcessName,"
                                   << "," << name << "\n";

    size_t process_hash = std::hash<std::string>{}(process);
    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(process_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    hash_map_[process_hash] = process;
    return simulationEventProcessName(time, process_hash, name);
}

ErrorCodes BtfFile::simulationEventProcessName(uint64_t time, size_t process_hash, const std::string& name)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(process_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    auto sim_hash = std::hash<std::string>{}("SIM");
    hash_map_[sim_hash] = "SIM";

    // emit event
    btf_entries_.push_back({time, EntityTypes::simulation, process_hash, 0, sim_hash, 0, Simulation::Events::tag, "COMM:" + name});
    btf_entries_per_entity_[process_hash].push_back((--btf_entries_.end()));

    return ErrorCodes::success;
}

ErrorCodes BtfFile::simulationEventProcessCreation(uint64_t time, const std::string& process, uint64_t pid, uint64_t ppid)
{
    printTrace() << time << "," << process << ","
                                   << "ProcessCreation," << pid << ",PID:" << pid << ",PPID:" << ppid << "\n";

    size_t process_hash = std::hash<std::string>{}(process);
    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(process_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    hash_map_[process_hash] = process;
    return simulationEventProcessCreation(time, process_hash, pid, ppid);
}

ErrorCodes BtfFile::simulationEventProcessCreation(uint64_t time, size_t process_hash, uint64_t pid, uint64_t ppid)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(process_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    auto sim_hash = std::hash<std::string>{}("SIM");
    hash_map_[sim_hash] = "SIM";

    // emit event
    btf_entries_.push_back({time, EntityTypes::simulation, process_hash, 0, sim_hash, 0, Simulation::Events::tag, "PID:" + std::to_string(pid)});
    btf_entries_per_entity_[process_hash].push_back((--btf_entries_.end()));
    btf_entries_.push_back({time, EntityTypes::simulation, process_hash, 0, sim_hash, 0, Simulation::Events::tag, "PPID:" + std::to_string(ppid)});
    btf_entries_per_entity_[process_hash].push_back((--btf_entries_.end()));

    return ErrorCodes::success;
}

ErrorCodes BtfFile::simulationEventThreadName(uint64_t time, const std::string& thread, const std::string& name)
{
    printTrace() << time << "," << thread << ","
                                   << "ThreadName," << name << "\n";

    size_t thread_hash = std::hash<std::string>{}(thread);
    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(thread_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    hash_map_[thread_hash] = thread;
    return simulationEventThreadName(time, thread_hash, name);
}

ErrorCodes BtfFile::simulationEventThreadName(uint64_t time, size_t thread_hash, const std::string& name)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(thread_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    auto sim_hash = std::hash<std::string>{}("SIM");
    hash_map_[sim_hash] = "SIM";

    // emit event
    btf_entries_.push_back({time, EntityTypes::simulation, thread_hash, 0, sim_hash, 0, Simulation::Events::tag, "COMM:" + name});
    btf_entries_per_entity_[thread_hash].push_back((--btf_entries_.end()));

    return ErrorCodes::success;
}

ErrorCodes BtfFile::simulationEventThreadCreation(uint64_t time, const std::string& thread, uint64_t tid, uint64_t pid)
{
    printTrace() << time << "," << thread << ","
                                   << "ThreadCreation,TID:" << tid << ",PID:" << pid << "\n";

    size_t thread_hash = std::hash<std::string>{}(thread);
    // we must do here a type check, otherwise we mess up our hash map
    ErrorCodes er = checkType(thread_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    hash_map_[thread_hash] = thread;
    return simulationEventThreadCreation(time, thread_hash, tid, pid);
}

ErrorCodes BtfFile::simulationEventThreadCreation(uint64_t time, size_t thread_hash, uint64_t tid, uint64_t pid)
{
    ErrorCodes er = checkTime(time);
    if (er != ErrorCodes::success)
    {
        return er;
    }
    er = checkType(thread_hash, EntityTypes::task);
    if (er != ErrorCodes::success)
    {
        return er;
    }

    auto sim_hash = std::hash<std::string>{}("SIM");
    hash_map_[sim_hash] = "SIM";

    // emit event
    btf_entries_.push_back({time, EntityTypes::simulation, thread_hash, 0, sim_hash, 0, Simulation::Events::tag, "TID:" + std::to_string(tid)});
    btf_entries_per_entity_[thread_hash].push_back((--btf_entries_.end()));
    btf_entries_.push_back({time, EntityTypes::simulation, thread_hash, 0, sim_hash, 0, Simulation::Events::tag, "PID:" + std::to_string(pid)});

    return ErrorCodes::success;
}

void BtfFile::setIgnoreMultipleTaskReleases(bool value)
{
    ignore_multiple_task_releases_ = value;
}

} // namespace btf