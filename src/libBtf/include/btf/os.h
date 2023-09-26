#pragma once

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