#include "btf/os.h"

namespace btf
{
std::string OS::eventToString(Events ev)
{
    switch (ev)
    {
    case btf::OS::Events::clear_event:
        return "clear_event";
        break;
    case btf::OS::Events::set_event:
        return "set_event";
        break;
    case btf::OS::Events::wait_event:
        return "wait_event";
        break;
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown event");
        break;
    }
}

OS::Events OS::stringToEvent(const std::string& str)
{
    OS::Events return_value{OS::Events::unknown};

    if (str == "clear_event")
    {
        return_value = OS::Events::clear_event;
    }
    else if (str == "set_event")
    {
        return_value = OS::Events::set_event;
    }
    else if (str == "wait_event")
    {
        return_value = OS::Events::wait_event;
    }

    return return_value;
}

bool OS::isWait(OS::Events e) const
{
    return e == Events::wait_event;
}
} // namespace btf