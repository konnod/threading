//
// Created by konnod on 5/3/18.
//

#ifndef THREADING_CONDITION_H
#define THREADING_CONDITION_H

#include <memory>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include "SPtrFactoryBase.h"
#include "../ThreadSafeBase.h"

#define WAKE_IF(w) [&] { return (w); }

/**
 * STL condition_variable wrapper.
 * https://en.cppreference.com/w/cpp/thread/condition_variable
 * > "The thread that intends to modify the variable has to
 * > 1. acquire a std::mutex (by acquireLock() method)
 * > 2. perform the modification while the lock is held
 * > 3. execute notify_one or notify_all on the std::condition_variable
 * > (the lock does not need to be held for notification) "
 * After acquireLock() there must be a call to releaseLock() before
 * notify*() call.
 */
class Condition final : public SPtrFactoryBase<Condition>, public ThreadSafeBase {
public:
    Condition() = default;
    Condition(const Condition&) = delete;
    const Condition &operator=(const Condition&) = delete;

    void notify_one() {
        cond.notify_one();
    }

    void notify_all() {
        cond.notify_all();
    }

    /**
     * @brief Blocks thread of execution until predicate evaluates to true
     * @param p predicate which returns ​false if the waiting should be continued.
     */
    template<typename Predicate>
    void wait(Predicate p) {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, p);
    }

    /**
     * @brief Blocks thread of execution for specified amount of time
     * until predicate evaluates to true or timeout expires
     * @param ms milliseconds to wait
     * @param p predicate which returns ​false if the waiting should be continued
     * @return false if the predicate @p still evaluates to false
     *         after the timeout expired, otherwise true
     */
    template<typename Rep, typename Period, typename Predicate>
    bool wait_for(const std::chrono::duration<Rep, Period>& time, Predicate p) {
        std::unique_lock<std::mutex> lock(mutex);
        return cond.wait_for(lock, time, p);
    }

protected:

    std::condition_variable cond;
};

#endif //THREADING_CONDITION_H
