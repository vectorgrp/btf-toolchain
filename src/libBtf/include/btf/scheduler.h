#pragma once

/* scheduler.h */

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