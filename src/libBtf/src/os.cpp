/* os.cpp */

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