/* common.cpp */

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

#include "btf/common.h"

namespace btf
{
std::string errorCodeToString(ErrorCodes code)
{
    switch (code)
    {
    case ErrorCodes::success:
        return "success";
    case ErrorCodes::already_in_state:
        return "already in state";
    case ErrorCodes::invalid_state_transition:
        return "invalid state transition";
    case ErrorCodes::descending_timestamp:
        return "descending timestamp";
    case ErrorCodes::invalid_type:
        return "invalid type";
    case ErrorCodes::invalid_event:
        return "invalid event type";
    case ErrorCodes::core_idle_task_still_running:
        return "core idle task still running";
    case ErrorCodes::multiple_tasks_running:
        return "multiple tasks running";
    case ErrorCodes::event_on_idle_core:
        return "event on idle core";
    case ErrorCodes::no_task_running:
        return "no task running";
    case ErrorCodes::terminate_on_runnable_with_running_sub_runnable:
        return "terminate on runnable with running sub runnable";
    case ErrorCodes::terminate_on_task_with_running_runnables:
        return "terminate on task with running runnables";
    case ErrorCodes::allocated_to_different_core:
        return "allocated to different core";
    case ErrorCodes::runnable_source_task_not_running:
        return "runnable not running on current running task";
    case ErrorCodes::source_and_target_not_equal:
        return "source and target of stimulus are not equal";
    case ErrorCodes::amount_of_semaphore_accesses_invalid:
        return "amount of semaphore accesses is invalid.";
    }
    FATAL_INTERNAL_ERROR_MSG("unknown error code");
}
} // namespace btf