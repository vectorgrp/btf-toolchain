#include "btf/runnable.h"

#include <string>

#include "btf/common.h"

namespace btf
{
Runnable::Runnable(States state) : state_(state)
{
}

ErrorCodes Runnable::doStateTransition(Events core_event)
{
    switch (state_)
    {
    case States::terminated:
        if (core_event == Events::terminate)
        {
            return ErrorCodes::already_in_state;
        }
        else if (core_event == Events::start)
        {
            state_ = States::running;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::running:
        if (core_event == Events::start || core_event == Events::resume)
        {
            return ErrorCodes::already_in_state;
        }
        else if (core_event == Events::suspend)
        {
            state_ = States::suspended;
        }
        else if (core_event == Events::terminate)
        {
            state_ = States::terminated;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::suspended:
        if (core_event == Events::suspend)
        {
            return ErrorCodes::already_in_state;
        }
        else if (core_event == Events::resume)
        {
            state_ = States::running;
        }
        else
        {
            return ErrorCodes::invalid_state_transition;
        }
        break;
    case States::unknown:
        if (core_event == Events::start || core_event == Events::resume)
        {
            state_ = States::running;
        }
        else if (core_event == Events::terminate)
        {
            state_ = States::terminated;
        }
        else if (core_event == Events::suspend)
        {
            state_ = States::suspended;
        }
        break;
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown state")
        break;
    }
    return ErrorCodes::success;
}

std::string Runnable::eventToString(Events ev)
{
    switch (ev)
    {
    case Events::start:
        return "start";
    case Events::terminate:
        return "terminate";
    case Events::resume:
        return "resume";
    case Events::suspend:
        return "suspend";
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown event")
    }
}

Runnable::Events Runnable::stringToEvent(const std::string& str)
{
    Runnable::Events return_value{Runnable::Events::unknown};

    if (str == "start")
    {
        return_value = Events::start;
    }
    else if (str == "terminate")
    {
        return_value = Events::terminate;
    }
    else if (str == "resume")
    {
        return_value = Events::resume;
    }
    else if (str == "suspend")
    {
        return_value = Events::suspend;
    }

    return return_value;
}

bool Runnable::isRunning() const
{
    return state_ == States::running;
}

bool Runnable::wasSuspendedByTaskPreempt() const
{
    return was_suspended_by_task_preempt_;
}

void Runnable::setWasSuspendedByTaskPreempt(bool val)
{
    was_suspended_by_task_preempt_ = val;
}

} // namespace btf