/* semaphore.cpp */

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

#include <string>

#include "btf/semaphore.h"
#include "btf/common.h"

namespace btf
{
Semaphore::Semaphore(States state) : state_(state)
{
}

ErrorCodes Semaphore::doStateTransition(Events ev)
{
    switch (state_)
    {
    case States::free:
        if (ev == Events::used)
        {
            state_ = States::used;
        }
        else if (ev == Events::lock)
        {
            state_ = States::full;
        }
        else if (ev == Events::free || ev == Events::unlock)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::full:
        if (ev == Events::unlock)
        {
            state_ = States::free;
        }
        else if (ev == Events::unlock_full)
        {
            state_ = States::used;
        }
        else if (ev == Events::overfull)
        {
            state_ = States::overfull;
        }
        else if (ev == Events::full || ev == Events::lock || ev == Events::lock_used)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::overfull:
        if (ev == Events::full)
        {
            state_ = States::full;
        }
        else if (ev == Events::overfull)
        {
            state_ = States::overfull;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::used:
        if (ev == Events::free)
        {
            state_ = States::free;
        }
        else if (ev == Events::lock_used)
        {
            state_ = States::full;
        }
        else if (ev == Events::used)
        {
            state_ = States::used;
        }
        else if (ev == Events::unlock_full)
        {
            return ErrorCodes::already_in_state;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::unknown:
        if (ev == Events::used || ev == Events::unlock_full)
        {
            state_ = States::used;
        }
        else if (ev == Events::free || ev == Events::unlock)
        {
            state_ = States::free;
        }
        else if (ev == Events::lock || ev == Events::lock_used || ev == Events::full)
        {
            state_ = States::full;
        }
        else if(ev == Events::overfull)
        {
            state_ = States::overfull;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    }
    return ErrorCodes::success;
}

std::string Semaphore::eventToString(Events ev)
{
    switch (ev)
    {
    case Events::assigned:
        return "assigned";
    case Events::decrement:
        return "decrement";
    case Events::free:
        return "free";
    case Events::full:
        return "full";
    case Events::increment:
        return "increment";
    case Events::lock:
        return "lock";
    case Events::lock_used:
        return "lock_used";
    case Events::overfull:
        return "overfull";
    case Events::queued:
        return "queued";
    case Events::released:
        return "released";
    case Events::requestsemaphore:
        return "requestsemaphore";
    case Events::unlock:
        return "unlock";
    case Events::unlock_full:
        return "unlock_full";
    case Events::used:
        return "used";
    case Events::waiting:
        return "waiting";
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown event")
    }
}

Semaphore::Events Semaphore::stringToEvent(const std::string& str)
{
    Semaphore::Events return_value{Events::unknown};

    if (str == "assigned")
    {
        return_value = Events::assigned;
    }
    else if (str == "decrement")
    {
        return_value = Events::decrement;
    }
    else if (str == "free")
    {
        return_value = Events::free;
    }
    else if (str == "full")
    {
        return_value = Events::full;
    }
    else if (str == "increment")
    {
        return_value = Events::increment;
    }
    else if (str == "lock")
    {
        return_value = Events::lock;
    }
    else if (str == "lock_used")
    {
        return_value = Events::lock_used;
    }
    else if (str == "overfull")
    {
        return_value = Events::overfull;
    }
    else if (str == "queued")
    {
        return_value = Events::queued;
    }
    else if (str == "released")
    {
        return_value = Events::released;
    }
    else if (str == "requestsemaphore")
    {
        return_value = Events::requestsemaphore;
    }
    else if (str == "unlock")
    {
        return_value = Events::unlock;
    }
    else if (str == "unlock_full")
    {
        return_value = Events::unlock_full;
    }
    else if (str == "used")
    {
        return_value = Events::used;
    }
    else if (str == "waiting")
    {
        return_value = Events::waiting;
    }

    return return_value;
}

} // namespace btf