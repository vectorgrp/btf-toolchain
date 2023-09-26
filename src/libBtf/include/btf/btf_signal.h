#pragma once

/* btf_signal.h */

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
  @brief Class for Signal (SIG) Events (see BTF Specification Chapter 2.3.6).

  A Signal (SIG) is basically an address in the memory of a micro-controller.
  This memory location contains a certain value, which can be accessed by a process instance if required. \n
  So generally, a signal fulfills the function of naming the memory space, like a label. 
  According to the stored value the process accessing it might change its behavior (READ). \n
  Besides a process also a stimulus is able to write to the storage location and change the value of this label (WRITE).
*/
class Signal
{
  public:
    /*!
      @brief Possible types of Signal events.
    */
    enum class Events
    {
        /// The read event indicates that a signal (target) gets read by a process (source).
        read,
        /// The write event indicates that a signal (target) gets written by a process or stimulus (source).
        write,
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