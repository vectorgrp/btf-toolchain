#include "btf/stimulus.h"

namespace btf
{
std::string Stimulus::eventToString(Events ev)
{
    switch (ev)
    {
    case btf::Stimulus::Events::trigger:
        return "trigger";
        break;
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown event");
        break;
    }
}

Stimulus::Events Stimulus::stringToEvent(const std::string& str)
{
    Stimulus::Events return_value{Stimulus::Events::unknown};

    if (str == "trigger")
    {
        return_value = Stimulus::Events::trigger;
    }

    return return_value;
}
} // namespace btf