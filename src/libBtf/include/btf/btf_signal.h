#pragma once

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