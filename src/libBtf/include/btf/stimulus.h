#pragma once

/* stimulus.h */

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
  @brief Class for Stimulus (STIM) Events (see BTF Specification Chapter 2.3.1).

  A Stimulus (STI) is used to represent interactions among the internal behavior and between
  the system and the surrounding environment.
*/
class Stimulus
{
  public:
    /*!
      @brief Possible types of Stimulus events.
    */
    enum class Events
    {
        /// The trigger event indicates that the internal behavior or the surrounding environment triggers \n
        /// the activation of a task/interrupt service routine or the setting of a signal value or OS-Event
        trigger,
        
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