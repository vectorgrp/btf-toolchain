#pragma once

#include <string>

#include "btf_entity_types.h"
#include "common.h"

namespace btf
{

/*!
  @brief Class for Semaphore (SEM) Events (see BTF Specification Chapter 2.3.7).

  If more than one process is able to access a common resource, it might be necessary to \n
  restrict the maximum amount of accesses in order to protect this resource from race conditions. \n 
  Therefore, the operating system provides the possibility to use a semaphore (SEM) (e.g. spinlock).
*/
class Semaphore
{
  public:
    /*!
      @brief Possible types of Semaphore events.
    */
    enum class Events
    {
        /// The assigned event indicates that a process (source) gets assigned to a semaphore (target) \n
        /// and does not have to wait for assignment.
        assigned,
        
        /// The decrement event indicates that a process (source) has released a semaphore (target) \n
        /// and now decrements its counter.
        decrement,

        /// The free event indicates that a semaphore (source and target) reaches 0 assigned users and \n
        /// was not full before.
        free,

        /// The full event indicates that a semaphore (source and target) gets released by a user and \n
        /// reaches its maximum amount of assignable semaphore users. In this case, no requesting \n
        /// user has to wait anymore.
        full,

        /// The increment event indicates that a process (source) has requested a semaphore (target) \n
        /// and now increments its counter.
        increment,

        /// The lock event indicates that a semaphore (source and target) is requested by a user and \n
        /// reaches its maximum amount of assignable semaphore users and had no user assigned \n
        /// before.
        lock,

        /// The lock_used event indicates that a semaphore (source and target) is requested by an \n
        /// additional user and reaches its maximum amount of assignable semaphore users.
        lock_used,

        ///The overfull event indicates that a semaphore (source and target) is requested by a user and \n
        /// has more simultaneous accesses as assignable. Therefore, at least one requesting user has to wait.
        overfull,

        /// The queued event indicates that a process (source) has requested a semaphore (target) and \n
        /// this request is queued to be either assigned or waiting later on.
        queued,

        /// The released event indicates that a process (source) that is assigned to a semaphore (target) \n
        /// releases this semaphore.
        released,
        
        /// The requestsemaphore event indicates that a process (source) requests a semaphore (target).
        requestsemaphore,

        /// The unlock event indicates that a semaphore (source and target) reaches 0 assigned users
        /// and was full before.
        unlock,

        /// The unlock_full event indicates that a semaphore (source and target) which is released by a user, \n
        /// still has other users assigned and was full before.
        unlock_full,
        
        /// The used event indicates that a semaphore (source and target) is requested and has user sassigned. \n
        /// The amount of assigned semaphore users is afterwards still less than the maximum amount of 
        /// assignable semaphore users.
        used,

        /// The waiting event indicates that a process (source) requests a semaphore (target). \n
        /// The semaphore has already reached the maximum amount of assignable semaphore users. \n
        /// Therefore, the request has to wait.
        waiting,

        /// Default value.
        unknown
    };

    /*!
      @brief Possible types of Process states.
    */
    enum class States
    {
        /// Semaphore has no assigned users.
        free,

        /// Semaphore has assigned requests and has reached its maximum amount of simultaneous accesses.
        full,

        /// Semaphore is locked and at least one request is waiting for the semaphore.
        overfull,

        /// Semaphore has assigned requests and is still able to handle at least one request.
        used,

        ///Default state.
        unknown
    };

    /*!
      @brief Constructor of the class Semaphore.
    */
    Semaphore(States state = States::unknown);

    /*!
    @brief Transitions the current state depending on the event.
    @param[in] ev Events enum that triggered the state transition.
    @return ErrorCodes enum.
    */
    ErrorCodes doStateTransition(Events ev);

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

private:
    /// States variable that keeps track of the current state.
    States state_;
};
} // namespace btf