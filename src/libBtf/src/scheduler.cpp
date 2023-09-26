#include "btf/scheduler.h"

namespace btf
{
std::string Scheduler::eventToString(Events ev)
{
    switch (ev)
    {
    case btf::Scheduler::Events::schedule:
        return "schedule";
        break;
    case btf::Scheduler::Events::schedulepoint:
        return "schedulepoint";
        break;
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown event");
        break;
    }
}

Scheduler::Events Scheduler::stringToEvent(const std::string& str)
{
    Scheduler::Events return_value{Scheduler::Events::unknown};

    if (str == "schedule")
    {
        return_value = Scheduler::Events::schedule;
    }
    else if(str == "schedulepoint")
    {
        return_value = Scheduler::Events::schedulepoint;
    }

    return return_value;
}
} // namespace btf