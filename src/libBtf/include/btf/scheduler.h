#pragma once

/* scheduler.h */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

#include <string>

#include "common.h"

namespace btf
{

/*!
  @brief Class for Scheduler (SCHED) Events (see BTF Specification Chapter 2.3.4).

  The scheduler (SCHED) is part of the operating system and manages one or multiple cores.
  It is responsible for the execution order of all mapped processes on those cores.
*/
class Scheduler
{
  public:
    /*!
      @brief Possible types of Scheduler events.
    */
    enum class Events
    {
        /// The schedule event indicates that the scheduler (target and source) makes a scheduling decision.
        schedule,
        /// The schedulepoint event indicates that a scheduler (target) is requested by a task (source) \n
        /// at a cooperative schedule point.
        schedulepoint,
        /// Default value.
        unknown
    };

    /*!
    @brief Converts the enum Events into string.
    @param[in] ev Events enum that will be converted.
    @return String if Events type is valid, else a fatal error is triggered.
    */
    static std::string eventToString(Events ev);

    /*!
    @brief Converts a string into the enum Events.
    @param[in] str String that will be converted.
    @return Events enum.
    */
    static Events stringToEvent(const std::string& str);
};
} // namespace btf