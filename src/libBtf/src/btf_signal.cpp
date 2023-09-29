/* btf_signal.h */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

#include "btf/btf_signal.h"

namespace btf
{
std::string Signal::eventToString(Events ev)
{
    switch (ev)
    {
    case btf::Signal::Events::read:
        return "read";
        break;
    case btf::Signal::Events::write:
        return "write";
        break;
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown event");
        break;
    }
}

Signal::Events Signal::stringToEvent(const std::string& str)
{
    Signal::Events return_value{Signal::Events::unknown};

    if (str == "write")
    {
        return_value = Signal::Events::write;
    }
    else if (str == "read")
    {
        return_value = Signal::Events::read;
    }

    return return_value;
}
} // namespace btf