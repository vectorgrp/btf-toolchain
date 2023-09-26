/* btf_entity_types.cpp */

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

#include "btf/btf_entity_types.h"

#include "btf/common.h"

namespace btf
{

std::string entityTypeToString(EntityTypes type)
{
    switch (type)
    {
    case EntityTypes::core:
        return "C";
    case EntityTypes::os:
        return "EVENT";
    case EntityTypes::task:
        return "T";
    case EntityTypes::isr:
        return "I";
    case EntityTypes::stimulus:
        return "STI";
    case EntityTypes::scheduler:
        return "SCHED";
    case EntityTypes::semaphore:
        return "SEM";
    case EntityTypes::runnable:
        return "R";
    case EntityTypes::signal:
        return "SIG";
    case EntityTypes::simulation:
        return "SIM";
    case EntityTypes::syscall:
        return "SYSC";
    case EntityTypes::thread:
        return "THR";
    default:
        FATAL_INTERNAL_ERROR_MSG("unknown type")
    }
}

EntityTypes stringToEntityType(const std::string& str)
{
    EntityTypes return_value{EntityTypes::unknown};
    if (str == "C")
    {
        return_value = EntityTypes::core;
    }
    else if (str == "EVENT")
    {
        return_value = EntityTypes::os;
    }
    else if (str == "T")
    {
        return_value = EntityTypes::task;
    }
    else if (str == "I")
    {
        return_value = EntityTypes::isr;
    }
    else if (str == "STI")
    {
        return_value = EntityTypes::stimulus;
    }
    else if (str == "SCHED")
    {
        return_value = EntityTypes::scheduler;
    }
    else if (str == "SEM")
    {
        return_value = EntityTypes::semaphore;
    }
    else if (str == "R")
    {
        return_value = EntityTypes::runnable;
    }
    else if (str == "SIG")
    {
        return_value = EntityTypes::signal;
    }
    else if (str == "SIM")
    {
        return_value = EntityTypes::simulation;
    }
    else if (str == "SYSC")
    {
        return_value = EntityTypes::syscall;
    }
    else if (str == "THR")
    {
        return_value = EntityTypes::thread;
    }

    return return_value;
}
} // namespace btf