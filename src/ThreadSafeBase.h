//
// Created by konnod on 6/3/19.
//

#ifndef THREADING_THREADSAFEBASE_H
#define THREADING_THREADSAFEBASE_H

#include <mutex>

/// @brief Base of thread safe class, this class is inheritable-only
class ThreadSafeBase {
 public:
    virtual inline void acquireLock() final { mutex.lock(); }
    virtual inline void releaseLock() final { mutex.unlock(); }
    virtual inline std::mutex& getLock() final { return mutex; }
 protected:
    ThreadSafeBase() = default;
    virtual ~ThreadSafeBase() = default;

protected:
    std::mutex mutex;
};

#endif //THREADING_THREADSAFEBASE_H
