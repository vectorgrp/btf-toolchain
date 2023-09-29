#pragma once

/* btf_entity_types.h */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

#include <string>
#include <unordered_set>

namespace btf
{

/*!
    @brief All types of entities that are supported by the btf lib.
*/
enum class EntityTypes
{
    /// "C"
    core,
    /// "EVENT"
    os,
    /// "T"
    task,
    /// "I"
    isr,
    /// STI
    stimulus,
    /// SCHED
    scheduler,
    /// SEM
    semaphore,
    /// "R"
    runnable,
    /// "SIG"
    signal,
    /// ---
    comment,
    /// "SIM"
    simulation,
    /// "SYSC"
    syscall,
    /// "THR"
    thread,
    /// default value
    unknown
};

/*!
    @brief Converts the enum EntityTypes into string.
    @param[in] type EntityType that will be converted.
    @return String if EntityType is valid, else a fatal error is triggered.
*/
std::string entityTypeToString(EntityTypes type);

/*!
    @brief Converts a string into the enum EntityTypes.
    @param[in] str String that will be converted.
    @return EntityType enum.
*/
EntityTypes stringToEntityType(const std::string& str);
} // namespace btf