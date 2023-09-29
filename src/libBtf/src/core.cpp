/* core.cpp */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

#include "btf/core.h"

namespace btf
{
Core::Core(States state) : state_(state)
{
}

ErrorCodes Core::doStateTransition(Events core_event)
{
    switch (state_)
    {
    case States::idle:
        if (core_event == Events::idle)
        {
            return ErrorCodes::already_in_state;
        }
        else if (core_event == Events::execute)
        {
            state_ = States::execution;
        }
        break;
    case States::execution:
        if (core_event == Events::execute)
        {
            return ErrorCodes::already_in_state;
        }
        else if (core_event == Events::idle)
        {
            state_ = States::idle;
        }
        break;
    case States::unknown:
        if (core_event == Events::execute)
        {
            state_ = States::execution;
        }
        else if (core_event == Events::idle)
        {
            state_ = States::idle;
        }
        break;
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown state")
    }
    return ErrorCodes::success;
}

std::string Core::eventToString(Events ev)
{
    switch (ev)
    {
    case Events::idle:
        return "idle";
    case Events::execute:
        return "execute";
    case Events::set_frequence:
        return "set_frequence";
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown event")
    }
}

Core::Events Core::stringToEvent(const std::string& str)
{
    Core::Events return_value{Core::Events::unknown};

    if (str == "idle")
    {
        return_value = Events::idle;
    }
    else if (str == "execute")
    {
        return_value = Events::execute;
    }
    else if (str == "set_frequence")
    {
        return_value = Events::set_frequence;
    }

    return return_value;
}

bool Core::isIdle() const
{
    return state_ == States::idle;
}

bool Core::isStateUnknown() const
{
    return state_ == States::unknown;
}

} // namespace btf