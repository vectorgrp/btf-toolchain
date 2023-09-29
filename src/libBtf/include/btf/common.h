#pragma once

/* common.h */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

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