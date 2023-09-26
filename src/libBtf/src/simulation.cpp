#include "btf/simulation.h"

namespace btf
{

std::string Simulation::eventToString(Events ev)
{
    if (ev == Events::tag)
    {
        return "tag";
    }
    FATAL_INTERNAL_ERROR_MSG("unknown event");
}
Simulation::Events Simulation::stringToEvent(const std::string& str)
{
    if (str == "tag")
    {
        return Simulation::Events::tag;
    }
    return Simulation::Events::unknown;
}
} // namespace btf