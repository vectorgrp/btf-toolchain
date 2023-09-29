#pragma once

/* runnable.h */

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
  @brief Class for Runnable Events (see BTF Specification Chapter 2.3.3).

  A runnable (R) is called in context of a process instance or by another runnable. \n
  A called runnable starts and changes to RUNNING. If the process instance which includes the runnable gets removed from core (e.g. preempted), \n 
  the currently executed runnable is suspended and changes to state SUSPENDED. If the process instance gets allocated to core (e.g. resumed), \n
  the runnable changes to RUNNING. After complete execution, the runnable changes to TERMINATED.
*/
class Runnable
{
  public:
    /*!
      @brief Possible types of runnable states.
    */
    enum class States
    {
        /// The runnable instance has finished execution or hasn’t been started yet.
        terminated,

        ///The runnable instance executes on a core.
        running,

        ///The runnable instance stops execution as the process instance has to be removed from core.
        suspended,

        /// Default state.
        unknown
    };

    /*!
      @brief Possible types of runnable events.
    */
    enum class Events
    {
        /// The start event indicates that a runnable (target) gets called by a process (source) and starts execution.
        start,

        /// The terminate event indicates that a runnable (target) that has been called by a process (source) has finished its execution.
        terminate,

        /// The suspend event indicates that a runnable (target) has to pause its execution as its calling process (source) gets preempted.
        suspend,

        /// The resume event indicates that a runnable (target) can continue execution as its process (source) resumes.
        resume,

        /// Default state.
        unknown
    };

    /*!
      @brief Constructor of the class Runnable.
    */
    Runnable(States state = States::unknown);

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
    @brief Checks, if the current runnable is in the state running.
    @return True if the current state is running, else false.
    */
    bool isRunning() const;

    /*!
    @brief Checks, if the current runnable is suspended due to a preemption.
    @return True if the runnable was suspended due to a preemption, else false.
    */
    bool wasSuspendedByTaskPreempt() const;

    /*!
    @brief Sets the variable "was_suspended_by_task_preempt".
    @param[in] val boolean value.
    */
    void setWasSuspendedByTaskPreempt(bool val);

  private:
    /// States variable that keeps track of the current state.
    States state_;

    /// Boolean value that is true when the process to which the runnable is assigned is suspended.
    bool was_suspended_by_task_preempt_{false};
};
} // namespace btf