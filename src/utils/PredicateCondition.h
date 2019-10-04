//
// Created by konnod on 5/16/18.
//

#ifndef THREADING_PREDICATECONDITION_H
#define THREADING_PREDICATECONDITION_H

#include <memory>
#include <mutex>
#include <condition_variable>

#include "SPtrFactoryBase.h"
#include "../ThreadSafeBase.h"

/**
 * @brief A Condition that is initialized with predicate
 * @tparam Predicate Predicate
 */
template<typename Predicate>
class PredicateCondition
        : public SPtrFactoryBase<PredicateCondition<Predicate>>
        , public ThreadSafeBase 
{
public:
    /**
     * @brief Ctor
     * @param p Predicate which returns ​false if the waiting should be continued
     */
    explicit PredicateCondition(Predicate p) : predicate(p) { }
    PredicateCondition(const PredicateCondition &) = delete;
    const PredicateCondition &operator=(const PredicateCondition &) = delete;

    /**
     * @brief Notifies one thread waiting on condition variable
     */
    void notify_one() {
        std::lock_guard<std::mutex> guard(mutex);
        cond.notify_one();
    }

    /**
     * @brief Notifies all threads waiting on condition variable
     */
    void notify_all() {
        std::lock_guard<std::mutex> guard(mutex);
        cond.notify_all();
    }

    /**
     * @brief Blocks thread of execution until predicate evaluates to true
     */
    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, predicate);
    }

    /**
     * @brief Blocks thread of execution for specified amount of time
     * until predicate evaluates to true or timeout expires
     * @param ms milliseconds to wait
     * @return false if the predicate still evaluates to false
     *         after the timeout expired, otherwise true
     */
    template<typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& time) {
        std::unique_lock<std::mutex> lock(mutex);
        return cond.wait_for(lock, time, predicate);
    }

private:
    Predicate predicate;
    std::condition_variable cond;
};

#endif //THREADING_PREDICATECONDITION_H
