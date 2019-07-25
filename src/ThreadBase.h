//
// Created by konnod on 1/10/19.
//

#ifndef THREADING_THREADBASE_H
#define THREADING_THREADBASE_H

#include <atomic>
#include <string>
#include <thread>
#include <system_error>

/**
 * @class ThreadBase
 * @brief Base class for threading support
 */
class ThreadBase {
    /**
     * @enum State
     * @brief Describes the state thread can be in
     */
    enum class State {
        Created = 0, ///< The state thread is created with
        Running,     ///< Indicates that thread was started via calling start()
        Stopped,     ///< Indicates that thread was stopped via calling stop()

        Failed,      /**< Indicates that thread has failed while executing threadFunction.
                      * This state must be explicitly set by the object via calling fail()
                      */
        Joined       ///< Indicates that thread has been successfully joined
    };
public:
    ThreadBase() : state(State::Created) { }

    /**
     * @brief Stops and joins the thread.
     *
     * Derived destructor is called first, so if derived class needs to
     * perform some cleanup and it needs thread not to be running,
     * derived destructor can call stop() and join() to stop thread with no harm
     * done by this base class destructor in further.
     */
    virtual ~ThreadBase() {
        stopThread();
        joinThread();
    }

    /**
     * @brief Starts thread
     *
     * Creates new thread object and starts executing threadFunction
     * if thread is not already running and was not joined yet.
     */
    virtual void startThread()
    {
        /*
         * test_and_set() on atomic_flag returns its previous state,
         * so it will return false if thread was not started already.
         * This also does not allow thread to be started until it was joined
         */
        if (!threadStarted.test_and_set(std::memory_order_relaxed))
        {
            state.store(State::Running, std::memory_order_release);
            thread = std::thread(&ThreadBase::threadLoop, this);
        }
    }

    /**
     * @brief Sets the state to State::Stopped.
     */
    virtual void stopThread()
    { state.store(State::Stopped, std::memory_order_relaxed); }

    /**
     * @brief Joins the thread to release resources.
     *
     * This method will wait until threadFunction finishes.
     * It will return immediately if another thread
     * has already called this method.
     */
    virtual void joinThread()
    {
        /*
         * This will not allow thread to be joined multiple times which can
         * lead to undefined behaviour
         */
        if (!threadJoined.test_and_set(std::memory_order_relaxed))
        {
            if (thread.joinable()) {
                try {
                    thread.join();
                } catch (const std::system_error &e) {
                    throw;
                }
            }
            state.store(State::Joined, std::memory_order_relaxed);
            threadStarted.clear(std::memory_order_relaxed);
            threadJoined.clear(std::memory_order_relaxed);
        }
    }

    /**
     * @return true if thread state is Created
     */
    bool isCreated() const noexcept
    { return state.load(std::memory_order_relaxed) == State::Created; }

    /**
     * @return true if thread state is Running
     */
    bool isRunning() const noexcept
    { return state.load(std::memory_order_relaxed) == State::Running; }

    /**
     * @return true if thread state is Stopped
     */
    bool isStopped() const noexcept
    { return state.load(std::memory_order_relaxed) == State::Stopped; }

    /**
     * @return true if thread state is Failed
     */
    bool isFailed() const noexcept
    { return state.load(std::memory_order_relaxed) == State::Failed; }

    /**
     * @return true if thread state is Joined
     */
    bool isJoined() const noexcept
    { return state.load(std::memory_order_relaxed) == State::Joined; }

protected:

    /**
     * @brief The function that will be run by thread.
     *
     * This method must monitor failed state by itself and return if it is set.
     */
    virtual void threadFunction()
    {
        beforeThreadLoop();
        threadLoop();
        afterThreadLoop();
    }

    /**
     * Called at the beginning, when the thread has just started
     * and didn't enter iteration loop
     */
    virtual void beforeThreadLoop() {}

    /**
     * Called before thread finishes, after iteration loop
     */
    virtual void afterThreadLoop() {}
    virtual void threadLoop()
    {
        while (true)
        {
            //Check state each time at the beginning of each iteration
            if (isFailed())
                break;
            if (isStopped())
                break;

            threadIteration();
        }
    }
    /**
     * @brief The iteration function, that is executed each time while thread is running
     */
    virtual void threadIteration() {};

    /**
     * @brief Sets the state to State::Failed.
     *
     * If threadFunction failed while executing its thread work,
     * it has to notify senior thread by calling this method and
     * then returning from threadFunction
     */
    void fail() noexcept
    { state.store(State::Failed, std::memory_order_relaxed); }

private:
    /// Underlying thread object
    std::thread thread;
    /// The flag that does not allow thread to be started twice
    std::atomic_flag threadStarted = ATOMIC_FLAG_INIT;
    /// The flag that does not allow thread to be joined twice
    std::atomic_flag threadJoined = ATOMIC_FLAG_INIT;
    /// The state of the thread
    std::atomic<State> state;
};


#endif //THREADING_THREADBASE_H
