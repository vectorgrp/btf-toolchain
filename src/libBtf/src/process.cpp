/* process.cpp */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

#include <string>

#include "btf/process.h"
#include "btf/common.h"

namespace btf
{
Process::Process(States state) : state_(state)
{
}

ErrorCodes Process::doStateTransition(Events ev)
{
    switch (state_)
    {
    case States::running:
        if (ev == Events::wait)
        {
            state_ = States::waiting;
        }
        else if (ev == Events::poll)
        {
            state_ = States::polling;
        }
        else if (ev == Events::preempt)
        {
            state_ = States::ready;
        }
        else if (ev == Events::terminate)
        {
            state_ = States::terminated;
        }
        else if (ev == Events::nowait)
        {
            state_ = States::running; // no transition
        }
        else if (ev == Events::start || ev == Events::resume || ev == Events::run)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::polling:
        if (ev == Events::run)
        {
            state_ = States::running;
        }
        else if (ev == Events::park)
        {
            state_ = States::parking;
        }
        else if (ev == Events::poll_parking || ev == Events::poll)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::waiting:
        if (ev == Events::release)
        {
            state_ = States::ready;
        }
        else if (ev == Events::wait)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::parking:
        if (ev == Events::poll_parking)
        {
            state_ = States::polling;
        }
        else if (ev == Events::release_parking)
        {
            state_ = States::ready;
        }
        else if (ev == Events::park)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::ready:
        if (ev == Events::resume)
        {
            state_ = States::running;
        }
        else if (ev == Events::preempt || ev == Events::release || ev == Events::release_parking)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::active:
        if (ev == Events::start)
        {
            state_ = States::running;
            was_started_ = true;
        }
        else if (ev == Events::interrupt_suspended)
        {
            state_ = States::active;
        }
        else if (ev == Events::activate)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::terminated:
        if (ev == Events::activate)
        {
            state_ = States::active;
        }
        else if (ev == Events::mtalimitexceeded)
        {
            state_ = States::terminated;
        }
        else if (ev == Events::terminate)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::unknown:
        if (ev == Events::activate || ev == Events::mtalimitexceeded)
        {
            state_ = States::active;
        }
        else if (ev == Events::start)
        {
            was_started_ = true;
            state_ = States::running;
        }
        else if (ev == Events::resume || ev == Events::run)
        {
            state_ = States::running;
        }
        else if (ev == Events::poll || ev == Events::poll_parking)
        {
            state_ = States::polling;
        }
        else if (ev == Events::park)
        {
            state_ = States::parking;
        }
        else if (ev == Events::release_parking || ev == Events::preempt || ev == Events::release)
        {
            state_ = States::ready;
        }
        else if (ev == Events::wait)
        {
            state_ = States::waiting;
        }
        else if (ev == Events::terminate || ev == Events::mtalimitexceeded)
        {
            state_ = States::terminated;
        }
        break;
    }
    return ErrorCodes::success;
}

std::string Process::eventToString(Events ev)
{
    switch (ev)
    {
    case Events::activate:
        return "activate";
    case Events::start:
        return "start";
    case Events::preempt:
        return "preempt";
    case Events::resume:
        return "resume";
    case Events::terminate:
        return "terminate";
    case Events::poll:
        return "poll";
    case Events::run:
        return "run";
    case Events::park:
        return "park";
    case Events::poll_parking:
        return "poll_parking";
    case Events::release_parking:
        return "release_parking";
    case Events::wait:
        return "wait";
    case Events::release:
        return "release";
    case Events::full_migration:
        return "fullmigration";
    case Events::enforced_migration:
        return "enforcedmigration";
    case Events::interrupt_suspended:
        return "interrupt_suspended";
    case Events::mtalimitexceeded:
        return "mtalimitexceeded";
    case Events::nowait:
        return "nowait";
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown event")
    }
}

Process::Events Process::stringToEvent(const std::string& str)
{
    Process::Events return_value{Events::unknown};

    if (str == "activate")
    {
        return_value = Events::activate;
    }
    else if (str == "start")
    {
        return_value = Events::start;
    }
    else if (str == "preempt")
    {
        return_value = Events::preempt;
    }
    else if (str == "resume")
    {
        return_value = Events::resume;
    }
    else if (str == "terminate")
    {
        return_value = Events::terminate;
    }
    else if (str == "poll")
    {
        return_value = Events::poll;
    }
    else if (str == "run")
    {
        return_value = Events::run;
    }
    else if (str == "park")
    {
        return_value = Events::park;
    }
    else if (str == "poll_parking")
    {
        return_value = Events::poll_parking;
    }
    else if (str == "release_parking")
    {
        return_value = Events::release_parking;
    }
    else if (str == "wait")
    {
        return_value = Events::wait;
    }
    else if (str == "release")
    {
        return_value = Events::release;
    }
    else if (str == "fullmigration")
    {
        return_value = Events::full_migration;
    }
    else if (str == "enforcedmigration")
    {
        return_value = Events::enforced_migration;
    }
    else if (str == "interrupt_suspended")
    {
        return_value = Events::interrupt_suspended;
    }
    else if (str == "mtalimitexceeded")
    {
        return_value = Events::mtalimitexceeded;
    }
    else if (str == "nowait")
    {
        return_value = Events::nowait;
    }

    return return_value;
}

EntityTypes Process::getSourceType(Events ev)
{
    switch (ev)
    {
    case Events::activate:
    case Events::mtalimitexceeded:
        return EntityTypes::stimulus;
    case Events::interrupt_suspended:
        return EntityTypes::scheduler;
    case Events::start:
    case Events::preempt:
    case Events::resume:
    case Events::terminate:
    case Events::poll:
    case Events::run:
    case Events::park:
    case Events::poll_parking:
    case Events::release_parking:
    case Events::wait:
    case Events::release:
    case Events::full_migration:
    case Events::enforced_migration:
    case Events::nowait:
        return EntityTypes::core;
    default:
        break;
    }
    FATAL_INTERNAL_ERROR_MSG("unknown event");
}

bool Process::isEventAllocatingCore(Events ev)
{
    switch (ev)
    {
    case Events::poll_parking:
    case Events::resume:
    case Events::start:
        return true;
    case Events::activate:
    case Events::mtalimitexceeded:
    case Events::interrupt_suspended:
    case Events::preempt:
    case Events::terminate:
    case Events::poll:
    case Events::run:
    case Events::park:
    case Events::release_parking:
    case Events::wait:
    case Events::release:
    case Events::full_migration:
    case Events::enforced_migration:
    case Events::nowait:
        return false;
    default:
        break;
    }
    FATAL_INTERNAL_ERROR_MSG("unknown event");
}

bool Process::isEventDeallocatingCore(Events ev)
{
    switch (ev)
    {
    case Events::park:
    case Events::wait:
    case Events::terminate:
    case Events::preempt:
        return true;
    case Events::poll_parking:
    case Events::resume:
    case Events::start:
    case Events::activate:
    case Events::mtalimitexceeded:
    case Events::interrupt_suspended:
    case Events::poll:
    case Events::run:
    case Events::release_parking:
    case Events::release:
    case Events::full_migration:
    case Events::enforced_migration:
    case Events::nowait:
        return false;
    default:
        break;
    }
    FATAL_INTERNAL_ERROR_MSG("unknown event");
}

bool Process::isTerminated() const
{
    return state_ == States::terminated;
}

bool Process::wasStarted() const
{
    return was_started_;
}

bool Process::waitOSevent() const
{
    return wait_os_;
}

void Process::setwaitOSevent(bool wait)
{
    wait_os_=wait;
}

} // namespace btf