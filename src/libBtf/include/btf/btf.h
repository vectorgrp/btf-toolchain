#pragma once

/* btf.h */

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

#include <algorithm>
#include <any>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "btf_entity_types.h"
#include "btf_signal.h"
#include "common.h"
#include "core.h"
#include "os.h"
#include "process.h"
#include "runnable.h"
#include "scheduler.h"
#include "semaphore.h"
#include "simulation.h"
#include "stimulus.h"

// Supported Entities
//	- Cores (only idle, execution and frequency change)
//  - OS
//	- Tasks/ISRs (Process)
//	- Runnables
//  - Scheduler
//  - Signals
//  - Stimuli

// Not supported general
//	- Entity Id Tables

// FIXME propably better to make the storage of events a vector (not a list), because faster and random accessable
//          requires to drop support for instertEvent

namespace btf
{

/*!
    @brief This struct generates a hash out of two values (uses a pair).
*/
struct PairHash
{
    template <typename T1, typename T2> std::size_t operator()(const std::pair<T1, T2>& p) const
    {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T1>{}(p.second);

        return (hash1 << 1) + hash1 + hash2;
    }
};

/*!
    @brief This struct generates a hash out of three values (uses a tuple).
*/
struct TupleHash
{
    template <typename T1, typename T2, typename T3> std::size_t operator()(const std::tuple<T1, T2, T3>& t) const
    {
        return std::get<0>(t)
               ^ std::get<1>(t)
               ^ std::get<2>(t);
    }
};

/*!
    @brief This struct generates a hash out of four values (uses a pair where the first parameter is a tuple).
*/
struct QuadrupleHash
{
    template <typename T1, typename T2, typename T3, typename T4> std::size_t operator()(const std::pair<std::tuple<T1, T2, T3>,T4>& q) const
    {
        return std::get<0>(q.first)
               ^ std::get<1>(q.first)
               ^ std::get<2>(q.first)
               ^ q.second;
    }
};

/*!
    @brief Class for a BtfEntry.

    A BtfEntry object is equivalent to a line in a BTF file.
*/
class BtfEntry
{
  public:
  
    /*!
        @brief Union Class for all supported Events.

        The union class type is chosen since a BtfEntry object can always only be one event type. \n
        Supported event types are Core, Process, Runnable, Signal and Simulation. \n
        The default constructor is assumed to be a Core event.
    */
    union Events {
        Core::Events core_event;
        OS::Events os_event;
        Process::Events process_event;
        Runnable::Events runnable_event;
        Scheduler::Events scheduler_event;
        Semaphore::Events semaphore_event;
        Signal::Events signal_event;
        Simulation::Events simulation_event;
        Stimulus::Events stimulus_event;

        /// @brief Default Constructor which is assumed to be a BtfEntry with a Core event.
        Events() : core_event(Core::Events::unknown){};

        /// @brief Constructor for a BtfEntry with a Core event.
        /// @param[in] ce Core event.
        Events(Core::Events ce) : core_event(ce){};
        
        /// @brief Constructor for a BtfEntry with an OS event.
        /// @param[in] oe OS event.
        Events(OS::Events oe) : os_event(oe){};

        /// @brief Constructor for a BtfEntry with a Process event.
        /// @param[in] te Process event.
        Events(Process::Events te) : process_event(te){};

        /// @brief Constructor for a BtfEntry with a Runnable event.
        /// @param re Runnable event.
        Events(Runnable::Events re) : runnable_event(re){};
        
        /// @brief Constructor for a BtfEntry with a Scheduler event.
        /// @param sche Scheduler event.
        Events(Scheduler::Events sche) : scheduler_event(sche){};
        
        /// @brief Constructor for a BtfEntry with a Semaphore event.
        /// @param sem Semaphore event.
        Events(Semaphore::Events sem) : semaphore_event(sem){};

        /// @brief Constructor for a BtfEntry with a Signal event.
        /// @param se Signal event.
        Events(Signal::Events se) : signal_event(se){};

        /// @brief Constructor for a BtfEntry with a Stimulus event.
        /// @param ste Schtimulus event.
        Events(Stimulus::Events ste) : stimulus_event(ste){};

        /// @brief Constructor for a BtfEntry with a Simulation event.
        /// @param se Simulation event.
        Events(Simulation::Events se) : simulation_event(se){};
    };

    /// Timestamp of the BtfEntry.
    uint64_t time_{0};

    /// Entity type of the BtfEntry. The default value is "unknown".
    EntityTypes type_{EntityTypes::unknown};

    /// Unique hash value of the source.
    size_t source_hash_{0};
    /// Source instance that triggered the event.
    uint64_t source_instance_{0};
    /// Unique hash value of the target.
    size_t target_hash_{0};
    /// Target instance of the event.
    uint64_t target_instance_{0};
    
    /// Events type of the BtfEntry.
    Events event_;

    /// Comments added to the BtfEntry.
    std::string note_;


    /*!
    @brief Converts a BtfEntry into string.
    @param[in] hash_map Unordered map which contains all hash values of the current BtfEntry.
    @return String if type_ and Events type are valid, else a fatal error is triggered.
    */
    std::string toString(std::unordered_map<size_t, std::string>& hash_map) const;

    /*!
    @brief Converts union Events into string according to the type_ of the BtfEntry.
    @return String if type_ and Events type are valid, else a fatal error is triggered.
    */
    std::string eventToString() const;
};

/*!
    @brief Main class of the btf lib.
    It contains all necessary functions to import and export a BTF file.
*/
class BtfFile
{
  public:
    
    /*!
        @brief Enum class which contains the possible time scales (pico-, nano-, micro- or milliseconds).
    */
    enum class TimeScales
    {
        /// 10<sup>-12</sup> s
        pico_seconds,
        /// 10<sup>-9</sup> s
        nano_seconds,
        /// 10<sup>-6</sup> s
        micro_seconds,
        /// 10<sup>-3</sup> s
        milli_seconds
    };

    /*!
        @brief Constructs a BTF file in memory.
        @param[in] path The path where the BTF file will be created.
        @param[in] time_scale The time unit in which the events are given.
        @param[in] auto_suspend_parent_runnable Boolean which, if true, causes parent runnables to automatically stop when a child runnable starts, 
                                                and resume when the child runnable finishes. The default value is false.\n
                                                Note: This option is not defined in the specification. However, it is included in the btf library 
                                                because it can later simplify the analysis of a btf trace with several child runnables.
        @param[in] source_is_core               Boolean which, if true, causes the osEvent, runnableEvent, signalEvent, schedulerEvent and semaphore event 
                                                to expect a core as source instead of a process (only relevant for event types for which a process 
                                                is defined as the source in the specification).
                                                Otherwise, a process is expected as source. The default value is true.\n
                                                Note: This option is not defined in the specification. According to the specification the source of these
                                                events are processes. However, since many trace formats only display which event occured on which core this
                                                option is implemented to ease the conversion from other trace formats to BTF.
        @param[in] auto_generate_core_events    Boolean which, if true, causes a core idle event to be automatically generated when no task is running 
                                                and the core to resume when a task runs again. The default value is false.\n
                                                Note: This option is not defined in the specification. However, it is included in the btf library 
                                                because it can later simplify the analysis of a btf trace with several child runnables.
        @param[in] auto_wait_resume_os_events   Boolean which, if true, causes the task state to be automatically updated after an OS event and 
                                                the corresponding task events to be generated. The default value is false.\n
                                                Note: This option is not defined in the specification. It is included for automation purposes.
    */
    explicit BtfFile(std::string path, TimeScales time_scale = TimeScales::nano_seconds, bool auto_suspend_parent_runnable = true,
                     bool source_is_core = true, bool auto_generate_core_events = false, bool auto_wait_resume_os_events = false);

    /*!
        @brief Writes all BtfEntry instances to file.
    */
    void finish();

    /*!
        @brief Appends the data from a BTF file. Currently only import into an empty BTF file is supported.
        @param[in] path The path to the BTF file.
        @param[in] delimiter The delimiter used in the BTF file.
    */
    void importFromFile(const std::string& path, char delimiter = ',');

    /*!
        @brief Sets the ID to name translation map. This should only be used for traces that use ID based APIs (e.g. for naming of events). \n
               Be careful when using this with string based APIs.
        @param[in] hash_map The ID to name translation map.
    */
    void setStringHashMap(std::unordered_map<size_t, std::string> hash_map);

    /*!
        @brief Emits a core event. See overloaded function for more information.
    */
    ErrorCodes coreEvent(uint64_t time, const std::string& core, Core::Events core_event);

    /*!
        @brief Emits a core event (source is a task).
        @param[in] time The timestamp of the event.
        @param[in] core_hash The ID of the core.
        @param[in] core_event The core event that occurred.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - core_idle_task_still_running: the core wants to go into the idle state but a task is still allocated to the core, \n
            - already_in_state: the core wants to go into a state it is already in.
    */
    ErrorCodes coreEvent(uint64_t time, size_t core_hash, Core::Events core_event);

        /*!
        @brief Emits an OS event (source is a core or a process depending on the value of the core_is_source parameter). See overloaded function for more information.
    */
    ErrorCodes osEvent(uint64_t time, const std::string& process, const std::string& os, OS::Events os_event);

    /*!
        @brief Emits an OS event (source is a core or a process depending on the value of the core_is_source parameter).
        @param[in] time The timestamp of the event.
        @param[in] core_hash The ID of the core on which the event occurred.
        @param[in] os_hash The ID of the OS instance that triggered the event.
        @param[in] os_event The OS event that occurred.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - core_idle_task_still_running: the core wants to go into the idle state but a task is still allocated to the core, \n
            - already_in_state: the core wants to go into a state it is already in.
    */
    ErrorCodes osEvent(uint64_t time, size_t core_hash, size_t os_hash, OS::Events os_event);


    /*!
        @brief Emits a task migration event. See overloaded function for more information.
    */
    ErrorCodes taskMigrationEvent(uint64_t time, const std::string& source_core, const std::string& destination_core, const std::string& task,
                                  uint64_t task_instance_id);

    /*!
        @brief Emits a task migration event.
        @param[in] time The timestamp of the event.
        @param[in] source_core_hash The ID of the core, on which the task was running.
        @param[in] destination_core_hash The ID of the core, on which the task will run.
        @param[in] task_hash The ID of the task.
        @param[in] task_instance_id The instance ID of the task.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - invalid_state_transition: the task is currently assigned to the core; in this state migration is not possible.
    */
    ErrorCodes taskMigrationEvent(uint64_t time, size_t source_core_hash, size_t destination_core_hash, size_t task_hash, uint64_t task_instance_id);

    /*!
        @brief Emits a task event (except migration). See overloaded function for more information.
    */
    ErrorCodes processEvent(uint64_t time, const std::string& source, const std::string& process, uint64_t process_instance_id, Process::Events process_event,
                         bool is_isr = false);

    /*!
        @brief Emits a process event (except task migration).
        @param[in] time The timestamp of the event.
        @param[in] source_hash The ID of the source on which the event occurred.
        @param[in] process_hash The ID of the process.
        @param[in] process_instance_id The instance ID of the process.
        @param[in] process_event The process event that occurred.
        @param[in] is_isr Boolean value that is true if the process is an ISR.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - event_on_idle_core: the core is currently idle, so no task event can occur, \n
            - invalid_state_transition: the intended state transition is invalid, \n
            - multiple_tasks_running: the task is allocated to a core which is already allocated by another task, \n
            - allocated_to_different_core: the task is currently allocated to a different core, \n
            - terminate_on_task_with_running_runnables: the task wants to terminate, but there are still runnables running in this task, \n
            - already_in_state: the target state of the task is already the current state.
    */
    ErrorCodes processEvent(uint64_t time, size_t source_hash, size_t process_hash, uint64_t process_instance_id, Process::Events process_event, bool is_isr = false);

    /*!
        @brief Emits a runnable event (source is a core or process depending on the value of the core_is_source parameter). See overloaded function for more information.
    */
    ErrorCodes runnableEvent(uint64_t time, const std::string& source, const std::string& runnable, Runnable::Events runnable_event);

    /*!
        @brief Emits a runnable event (source is a core or process depending on the value of the core_is_source parameter).
        @param[in] time The timestamp of the event.
        @param[in] core_hash The ID of the core on which the event occurred.
        @param[in] process_hash The ID of the process on which the event occurred.
        @param[in] runnable_hash The ID of the runnable.
        @param[in] runnable_event The runnable event that occurred.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - no_task_running: there is no task running on this core, therefore no runnable event can occure, \n
            - runnable_source_task_not_running: the runnable is not running on the current running task, \n
            - terminate_on_runnable_with_running_sub_runnable: the runnable wants to terminate, but there are still running sub-runnables, \n
            - invalid_state_transition: the intended state transition is invalid, \n
            - already_in_state: the target state of the runnable is already the current state.
    */
    ErrorCodes runnableEvent(uint64_t time, size_t core_hash, size_t process_hash, size_t runnable_hash, Runnable::Events runnable_event);

    /*!
        @brief Emits a semaphore event. See overloaded function for more information.
    */
    ErrorCodes semaphoreEvent(uint64_t time, const std::string& source, const std::string& target, Semaphore::Events semaphore_event, uint64_t note);

    /*!
        @brief Emits a semaphore event (source is a semaphore).
        @param[in] time The timestamp of the event.
        @param[in] semaphore_hash The ID of the semaphore.
        @param[in] semaphore_event The semaphore event that occurred.
        @param[in] note The amount of accesses on the semaphore.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - invalid_event: the specified event cannot be issued by the given source. \n
            - no_task_running: there is no task running on this core, therefore no signal event can occure, \n
            - invalid_state_transition: the intended state transition is invalid, \n
            - already_in_state: the target state of the runnable is already the current state.
    */
    ErrorCodes semaphoreEvent(uint64_t time, size_t semaphore_hash, Semaphore::Events semaphore_event, uint64_t note);

    /*!
        @brief Emits a semaphore event (source is a core or process depending on the value of the core_is_source parameter).
        @param[in] time The timestamp of the event.
        @param[in] core_hash The ID of the core on which the event occurred.
        @param[in] semaphore_hash The ID of the semaphore.
        @param[in] semaphore_event The semaphore event that occurred.
        @param[in] note The amount of accesses on the semaphore.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - invalid_event: the specified event cannot be issued by the given source, \n
            - no_task_running: there is no task running on this core, therefore no signal event can occure, \n
            - invalid_state_transition: the intended state transition is invalid, \n
            - already_in_state: the target state of the runnable is already the current state, \n
            - amount_of_semaphore_accesses_invalid: the given amount of semaphore accesses is invalid.
    */
    ErrorCodes semaphoreEvent(uint64_t time, size_t core_hash, size_t semaphore_hash, Semaphore::Events semaphore_event, uint64_t note);


    /*!
        @brief Emits a scheduler event. See overloaded function for more information.
    */
    ErrorCodes schedulerEvent(uint64_t time, const std::string& source, const std::string& scheduler, Scheduler::Events scheduler_event);

    /*!
        @brief Emits a scheduler event (source is a core or process depending on the value of the core_is_source parameter).
        @param[in] time The timestamp of the event.
        @param[in] core_hash The ID of the core on which the event occurred.
        @param[in] scheduler_hash The ID of the scheduler.
        @param[in] scheduler_event The scheduler event that occurred.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - invalid_event: the specified event cannot be issued by the given source, \n
            - no_task_running: there is no task running on this core, therefore no signal event can occure.
    */
    ErrorCodes schedulerEvent(uint64_t time, size_t core_hash, size_t scheduler_hash, Scheduler::Events scheduler_event);

        /*!
        @brief Emits a scheduler event (source is a scheduler).
        @param[in] time The timestamp of the event.
        @param[in] scheduler_hash The ID of the scheduler.
        @param[in] scheduler_event The scheduler event that occurred.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - invalid_event: the specified event cannot be issued by the given source, \n
            - no_task_running: there is no task running on this core, therefore no signal event can occure.
    */
    ErrorCodes schedulerEvent(uint64_t time, size_t scheduler_hash, Scheduler::Events scheduler_event);

    /*!
        @brief Emits a signal event (source is a core or process depending on the value of the core_is_source parameter). See overloaded function for more information.
    */
    ErrorCodes signalEvent(uint64_t time, const std::string& source, const std::string& signal, Signal::Events signal_event,
                           const std::string& signal_value = "");

    /*!
        @brief Emits a signal event (source is a core or process depending on the value of the core_is_source parameter).
        @param[in] time The timestamp of the event.
        @param[in] core_hash The ID of the core on which the event occurred.
        @param[in] signal_hash The ID of the signal.
        @param[in] signal_event The signal event that occurred.
        @param[in] signal_value (optional) value of the signal.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - no_task_running: there is no task running on this core, therefore no signal event can occure.
    */
    ErrorCodes signalEvent(uint64_t time, size_t core_hash, size_t signal_hash, Signal::Events signal_event, const std::string& signal_value = "");
    
    /*!
        @brief Emits a stimulus event. See overloaded function for more information.
    */
    ErrorCodes stimulusEvent(uint64_t time, const std::string& source, const std::string& target, Stimulus::Events stimulus_event);

    /*!
        @brief Emits a stimulus event.
        @param[in] time The timestamp of the event.
        @param[in] stimulus_hash The ID of the stimulus that triggered the event.
        @param[in] stimulus_event The stimulus event that occurred.
        @return If no error: success. Otherwise, the following errors are possible: \n
            - descending_timestamp: the timestamp is earlier than the timestamp of the last event, \n
            - invalid_type: the specified ID has already been used with a type other than the current one, \n
            - source_and_target_not_equal: The source and the target of the stimulus are not equal.
    */
    ErrorCodes stimulusEvent(uint64_t time, size_t stimulus_hash, Stimulus::Events stimulus_event);
    
    
    std::list<std::list<BtfEntry>::iterator> getEventsForEntity(const std::string& entity);
    std::list<std::list<BtfEntry>::iterator> getEventsForEntity(size_t entity_hash);

    /*!
        @brief Emits a comment.
        @param[in] comment The comment in form of a string. The # at the beginning is added by this function.
    */
    void comment(const std::string& comment);

    /*!
        @brief Writes a custom entry to the header.
        @param[in] header_entry The header entry.
    */
    void headerEntry(const std::string& header_entry);

    /*!
        @brief Inserts an event into the BTF trace. \n 
           \b Warning: Use on own risk. No checks are performed and 
                       the insertion of a wrong event can lead to an invalid BTF file.
        @param[in] pos The event is inserted before this position.
        @param[in] entry The BTF entry to insert.
    */
    void insertEvent(std::list<std::list<BtfEntry>::iterator>::iterator pos, const BtfEntry& entry);

    /*!
        @brief Gets the list of events for a entity.
        @param[in] entity The entity.
        @return The list of events for that entity.
    */
    std::list<std::list<BtfEntry>::iterator>& getEntityEvents(const std::string& entity);

    /*!
        @brief Gets a list of all events.
        @return All events.
    */
    std::list<BtfEntry> getAllEvents() const;

    /*!
        @brief Gets the number of all events.
        @return The number of events.
    */
    size_t getNumberOfAllEvents() const;

    /*!
        @brief Emits a process name event (e.g. name change or initial name). See overloaded function for more information.
    */
    ErrorCodes simulationEventProcessName(uint64_t time, const std::string& process, const std::string& name);

    /*!
        @brief Emits a process name event (e.g. name change or initial name).
        @param[in] time The timestamp of the event.
        @param[in] process_hash The ID of the process.
        @param[in] name The name of the process.
    */
    ErrorCodes simulationEventProcessName(uint64_t time, size_t process_hash, const std::string& name);

    /*!
        @brief Emits a process creation event. See overloaded function for more information.
    */
    ErrorCodes simulationEventProcessCreation(uint64_t time, const std::string& process, uint64_t pid, uint64_t ppid);

    /*!
        @brief Emits a process creation event, e.g. PID and PPID mapping to a process.
        @param[in] time The timestamp of the event.
        @param[in] process_hash The ID of the process.
        @param[in] pid The process ID of the process.
        @param[in] ppid The parent process ID of the parent process.
    */
    ErrorCodes simulationEventProcessCreation(uint64_t time, size_t process_hash, uint64_t pid, uint64_t ppid);

    /*!
        @brief Emits a thread name event (e.g. name change or initial name). See overloaded function for more information.
    */
    ErrorCodes simulationEventThreadName(uint64_t time, const std::string& thread, const std::string& name);

    /*!
        @brief Emits a thread name event (e.g. name change or initial name).
        @param[in] time The timestamp of the event.
        @param[in] thread_hash The ID of the thread.
        @param[in] name The name of the thread.
    */
    ErrorCodes simulationEventThreadName(uint64_t time, size_t thread_hash, const std::string& name);

    /*!
        @brief Emits a thread creation event. See overloaded function for more information.
    */
    ErrorCodes simulationEventThreadCreation(uint64_t time, const std::string& thread, uint64_t tid, uint64_t pid);

    /*!
        @brief Emits a thread creation event, e.g. TID and PID mapping to a thread.
        @param[in] time The timestamp of the event.
        @param[in] thread_hash The ID of the thread.
        @param[in] tid The Thread ID of the thread.
        @param[in] pid The Process ID of the thread.
    */
    ErrorCodes simulationEventThreadCreation(uint64_t time, size_t thread_hash, uint64_t tid, uint64_t pid);

    /*!
       @brief Sets the option to ignore multiple releases on a waiting task.
       @param[in] value If true ignore multiple releases on a waiting task, else consider them.
     */
    void setIgnoreMultipleTaskReleases(bool value);

  private:
    /// Delete the Copy Constructor.
    BtfFile(BtfFile&) = delete;
    /// Delete the Move Constructor.
    BtfFile(BtfFile&&) = delete;

    /// Delete the copy assignment operator.
    void operator=(BtfFile&) = delete;
    /// Delete the move assignment operator.
    void operator=(BtfFile&&) = delete;

    /*!
       @brief Generates a Core event with the event type idle.
       @param[in] time The timestamp of the event.
       @param[in] source_hash The ID of the source.
    */
    void generateCoreIdleEvent(uint64_t time, size_t source_hash);

    /*!
       @brief Generates a Core event with the event type execute.
       @param[in] time The timestamp of the event.
       @param[in] source_hash The ID of the source.
       @param[in] process_event The process event that triggered the core event.
    */
    void generateCoreExecuteEvent(uint64_t time, size_t source_hash, Process::Events process_event);

    /*!
       @brief Checks if the timestamps are ascending.
       @param[in] time The timestamp of the event.
       @return Success if timestamps are ascending, else descending_timestamp ErrorCode.
    */
    ErrorCodes checkTime(uint64_t time);

    /*!
       @brief Checks if the type is correct.
       @param[in] hash The ID of the event to be checked.
       @param[in] should_be_type The event type the event should have.
       @return Success if timestamps are ascending, else invalid_type ErrorCode.
    */
    ErrorCodes checkType(size_t hash, EntityTypes should_be_type);

    /*!
       @brief Gets the Header of the BTF trace.
       @return Returns the three header lines as string.
    */
    std::string getHeader() const;



    /// The path to the output BTF file.
    std::string path_;

    /// The timescale of the BTF trace.
    TimeScales time_scale_;

    /// The timestamp of the last event.
    uint64_t last_time_{0};

    /// Unordered map that keeps track of the current state of the cores.
    std::unordered_map<size_t, Core> cores_;

    /// Unordered map that keeps track of the current state of the tasks: pair of hash and instance id as key.
    std::unordered_map<std::pair<size_t, uint64_t>, Process, PairHash> tasks_;

    /// Unordered map that keeps track of the current state of the runnables: pair of hash and instance id as key.
    std::unordered_map<std::pair<size_t, size_t>, Runnable, PairHash> runnables_;

    /// Unordered map that keeps track of the current state of the semaphores: only the hash value since the instance id is always 0.
    std::unordered_map<size_t, Semaphore> semaphores_;

    /// Unordered map that keeps track which task belongs to which core.
    std::unordered_map<std::string, std::string> task_core_map_;

    /// Unordered map that keeps track of the runnables per task instance.
    std::unordered_map<std::pair<size_t, uint64_t>, std::vector<std::pair<size_t, uint64_t>>, PairHash> runnable_stacks_;

    /// Unordered map that keeps track of the os_wait state of the tasks.
    std::unordered_map<std::pair<std::tuple<size_t, uint64_t, size_t>, size_t>, bool, QuadrupleHash> os_iswait_;

    /// Unordered map that keeps track of all strings. It contains all events that occurred.
    std::unordered_map<size_t, std::string> hash_map_;

    /// List that contains all BtfEntry objects.
    std::list<BtfEntry> btf_entries_;

    /// Unordered map that keeps track of the BtfEntry objects per entity.
    std::unordered_map<size_t, std::list<std::list<BtfEntry>::iterator>> btf_entries_per_entity_;

    /// Unordered map that keeps track of the type for each object (e.g. the hashed name).
    std::unordered_map<size_t, EntityTypes> type_map_; 

    /// Unordered map that keeps track of the current running task and its instance ID (0,0 for no task) for each core.
    std::unordered_map<size_t, std::pair<size_t, size_t>> current_running_tasks_;

    /// Pair value that is used if no task is running on a core.
    const std::pair<size_t, size_t> no_running_task_{0, 0};

    /// Unordered map that keeps track for each core if any allocating or deallocating task event occurred. 
    std::unordered_map<size_t, bool> did_de_allocated_task_event_occurred_on_core_; 

    /// Unordered map that keeps track of the instance id for each stimulus.
    std::unordered_map<size_t, uint64_t> stimuli_instance_ids_map_;

    /// Unordered map that keeps track of the runnable events and their positions that occurred before a task event.
    std::unordered_map<size_t, std::vector<std::list<BtfEntry>::iterator>> runnable_without_task_buffers_; 
    
    /// Unordered map that keeps track of the runnable stack in case no task event occurred.
    std::unordered_map<size_t, std::vector<std::pair<size_t, uint64_t>>> runnable_without_task_stacks_;

    /// Unordered map that keeps track of the instance id counters for runnables.
    std::unordered_map<size_t, uint64_t> runnable_instance_id_counters_;

    /// Unordered map that stores custom header entries.
    std::vector<std::string> custom_header_entries_;

    /// Unordered map that keeps track if a task or core event occurred on a core.
    std::unordered_map<size_t, bool> did_task_allocation_event_happen_on_core_;

    /// Boolean value that is true when parent runnables are automatically suspended at the start of a sub-runnable.
    bool auto_suspend_parent_runnable_;

    /// Boolean which, if true, causes the osEvent, runnableEvent and signalEvent to expect a core as source. Otherwise, a process is expected as source.
    bool source_is_core_;

    ///  Boolean value that is true when core events (idle and execute) are automatically generated.
    bool auto_generate_core_events_;

    /// Boolean value that is true when task events (wait and resume) are automatically generated after an OS event occurred.
    bool auto_wait_resume_os_events_;

    /// Boolean value that is true when events are auto generated.
    bool auto_generate_events_{true};

    ///  Boolean value that is false when multiple releases on task in state ready are ignored.
    bool ignore_multiple_task_releases_{ false };
};
} // namespace btf
