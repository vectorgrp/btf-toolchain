/* btf_entity_types.cpp */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

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