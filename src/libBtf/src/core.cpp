/* core.cpp */

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