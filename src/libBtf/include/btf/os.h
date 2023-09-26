#pragma once

/* os.h */

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
  @brief Class for OS (EVENT) Events (see BTF Specification Chapter 2.3.5).

  OS-Events (EVENT) are objects provided by the operating system. They offer a possibility to \n
  synchronize different processes. An OS-Event is always associated with a task, which ”owns” \n
  the event.
*/
class OS
{
  public:
    /*!
      @brief Possible types of OS events.
    */
    enum class Events
    {
        /// The clear_event event indicates that a potentially set OS-Event (target) gets reset by the
        /// task owning this OS-Event.
        clear_event,
        
        /// The set_event event indicates that an OS-Event (target) gets set by a process or a stimulus (source).
        set_event,

        /// The wait_event event indicates that a task calls a system service to wait for an OS-Event.
        wait_event,

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

        /*!
    @brief Checks, if the current OS event is wait_event.
    @param[in] e OS event that is checked. 
    @return True if the OS event is the wait_event, else false.
    */
    bool isWait(OS::Events e) const;
};
} // namespace btf