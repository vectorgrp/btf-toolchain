#pragma once

#include "btf_entity_types.h"
#include "common.h"

namespace btf
{

/*!
  @brief Class for Process Events (see BTF Specification Chapter 2.3.2).

  A Process can be either a task (T) or an interrupt service routine (I). \n
  It is activated by a stimulus. After activation, a scheduler assigns the process to a core where the process is executed
*/
class Process
{
  public:
    /*!
      @brief Possible types of Process events.
    */
    enum class Events
    {
        /// The activate event indicates that a process transitions from TERMINATED to ACTIVE state.
        activate,

        /// The start event indicates that a process (target) that is active for execution gets scheduled and allocated to a core (source).
        start,

        /// The preempt event indicates that a process (target) that is executing on a core (source) gets removed from it due to a scheduling decision.
        preempt,

        /// The resume event indicates that a process (target) that has been removed from a core by a scheduler gets allocated to a core (source) due to a new scheduling decision.
        resume,

        /// The terminate event indicates that a process (target) has finished its execution.
        terminate,

        /// The poll event indicates that a request resource is not available and the process (target) starts waiting actively to access it.
        poll,

        /// The run event indicates that a process (target) that is actively waiting for a resource on a core (source) gets access to this resource and continues execution.
        run,

        /// The park event indicates that a process (target) that is actively waiting for a resource is suspended.
        park,

        /// The poll_parking event indicates that a process (target) that has been removed from the allocated core (source) during actively waiting for a resource 
        /// gets allocated to a core (source). During this reallocation, the requested resource is still not available.
        poll_parking,

        /// The release_parking event indicates that a process (target) that has been removed from the allocated core (source) during actively waiting for a resource 
        /// gets access to this resource. During this time, the process is still removed from the core due to a scheduling decision.
        release_parking,

        /// The wait event indicates that a process (target) calls a system service to wait for at least one OS-Event and these events are not set. 
        /// The process gets removed from the core (source) it is allocated to.
        wait,

        /// The release event indicates that at least one OS-Event a process (target) is passively waiting for is set and the process is ready to proceed execution.
        release,

        /// ---
        full_migration,

        /// ---
        enforced_migration,

        /// The interrupt_suspended event indicates that the stated interrupt service routine instance (target) is suspended.
        interrupt_suspended,

        /// The mtalimitexceeded event indicates that the maximum allowed number of concurrent activations of this task is already reached.
        mtalimitexceeded,

        /// ---
        nowait,

        /// Default value.
        unknown
    };

    /*!
      @brief Possible types of Process states.
    */
    enum class States
    {
        /// The process instance executes on a core.
        running,

        ///The process instance has requested a not available resource and waits actively.
        polling,

        ///The process instance has called the system service to wait for an OS-Event which is not set and waits passively.
        waiting,

        ///The process instance has been preempted while actively waiting for a resource that is not available.
        parking,

        ///The process instance has been preempted.
        ready,

        ///The process instance is ready for execution.
        active,

        ///The process instance has finished its execution or hasn’t been activated yet.
        terminated,

        ///Default state.
        unknown
    };

    /*!
      @brief Constructor of the class Process.
    */
    Process(States state = States::unknown);

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

    /*!
    @brief Gets the source type of the event.
    @param[in] ev Events enum which source is to be retrieved.
    @return EntityTypes enum.
    */
    static EntityTypes getSourceType(Events ev);

    /*!
    @brief Checks, if the event will lead to an allocation on the core.
    @param[in] ev Events enum that will be checked.
    @return True if event allocates on the core, else false.
    */
    static bool isEventAllocatingCore(Events ev);

    /*!
    @brief Checks, if the event will lead to an deallocation from the core.
    @param[in] ev Events enum that will be checked.
    @return True if event deallocates from the core, else false.
    */
    static bool isEventDeallocatingCore(Events ev);

    /*!
    @brief Checks, if the current process state is terminated.
    @return True if the current state is terminated, else false.
    */
    bool isTerminated() const;

    /*!
    @brief Checks, if the current process got a start event at some point.
    @return True if the process had a start event, else false.
    */
    bool wasStarted() const;

    /*!
    @brief Checks, if the current process is waiting for an OS event.
    @return True if the process waits for an OS event, else false.
    */
    bool waitOSevent() const;

    /*!
    @brief Allows to set the OS wait state of the process.
    @param[in] wait True if the process waits for an OS event, else false.
    */
    void setwaitOSevent(bool wait);

  private:
    /// States variable that keeps track of the current state.
    States state_;
    
    /// Boolean value that is true when the process was started.
    bool was_started_{false};

    /// Boolean value that is true when the process is waiting for an OS event.
    bool wait_os_{false};
};
} // namespace btf