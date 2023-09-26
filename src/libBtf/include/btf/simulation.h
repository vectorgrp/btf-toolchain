#pragma once

#include <string>

#include "common.h"

namespace btf
{

/*!
  @brief Class for the Simulation (SIM) events.
  
  Simulation events provide additional information, such as model information from the simulation environment.
*/
class Simulation
{
  public:
  /*!
  @brief Possible types of simulation events.
  */
    enum class Events
    {
        /// The tag event is used to annotate the BTF trace with system and model information.
        tag,
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