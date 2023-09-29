#pragma once

/* core.h */

/* 
 * Copyright (c) 2023 Vector Informatik GmbH
 *
 * SPDX-License-Identifier: MIT
*/

#include "common.h"

namespace btf
{

/*!
  @brief Class for the core (C) events.
  
  Core events indicate if interrupts are suspended on a core.
*/
class Core
{
  public:

    /*!
    @brief Possible types of core states.
    */
    enum class States
    {
        /// The core is currently idle.
        idle,

        /// The core is currently executing a process instance.
        execution,

        /// Default state.
        unknown
    };

    /*!
    @brief Possible types of core events.
    */
    enum class Events
    {
        /// The idle event indicates that the core is going into the idle state.
        idle,

        /// The execute event indicates that the core is going into the execution state.
        execute,

        /// ---
        set_frequence,

        /// Default state.
        unknown
    };

    /*!
      @brief Constructor of the class Core.
    */
    Core(States state = States::unknown);

    /*!
    @brief Transitions the current state depending on the event.
    @param[in] core_event Events enum that triggered the state transition.
    @return ErrorCodes enum.
    */
    ErrorCodes doStateTransition(Events core_event);

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

    /*!
    @brief Checks, if the current core is in the state idle.
    @return True if the current state is idle, else false.
    */
    bool isIdle() const;

    /*!
    @brief Checks, if the current core is in the state unknown.
    @return True if the current state is unknown, else false.
    */
    bool isStateUnknown() const;

  private:
    /// States variable that keeps track of the current state.
    States state_;
};
} // namespace btf