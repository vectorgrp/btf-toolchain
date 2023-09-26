#pragma once

/* common.h */

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

#include <iostream>

#include "helper/helper.h"

namespace btf
{

/*!
 @brief Error Codes for the btf library
 */
enum class ErrorCodes
{
    /// No error occured.
    success, 

    /// Entity wants to go in a state, in which it already was.
    already_in_state,     

    /// The state transition of an entity is not valid.
    invalid_state_transition, 

    /// The timestamp of an event is smaller than the timestamp of the last event.
    descending_timestamp, 

    /// The type of the object is invalid, e.g. the entity was previously used as another type.
    invalid_type,         

    /// The type of the event is invalid, e.g. the given source cannot issue that event.
    invalid_event,

    /// A core wants to switch to idle while a task is still running on it.
    core_idle_task_still_running, 

    /// Multiple tasks are running at the same time on the same core.
    multiple_tasks_running, 

    /// An Event appeared on an idle core.
    event_on_idle_core,        

    /// The source task of a runnable is not running.
    runnable_source_task_not_running, 

    /// No task is running, hence no runnable event is possible.
    no_task_running,                              

    /// The system tried to terminate a runnable with at least one running sub-runnable.
    terminate_on_runnable_with_running_sub_runnable, 

    /// The system tried to terminate a task, which has running runnables.
    terminate_on_task_with_running_runnables, 

    /// The entity is currently running on a different core.
    allocated_to_different_core,       

    /// Although required, the source and target are not equal.
    source_and_target_not_equal,

    // The given amount of semaphore accesses is invalid (see specification for more details).
    amount_of_semaphore_accesses_invalid        
};

/*!
    @brief Converts the enum ErrorCode into string.
    @param[in] code ErrorCode that will be converted.
    @return String if ErrorCode is valid, else a fatal error is triggered.
*/
std::string errorCodeToString(ErrorCodes code);
} // namespace btf