//
// Created by konnod on 11/12/18.
//

#ifndef THREADING_THREADPOOLHANDLERBASE_H
#define THREADING_THREADPOOLHANDLERBASE_H

#include <memory>
#include <vector>

template<typename _ThreadType>
class ThreadPoolBase {
public:
    typedef _ThreadType ThreadType;
    typedef std::shared_ptr<ThreadType> ThreadTypePtr;

    template<typename... Args>
    explicit ThreadPoolBase(unsigned int poolSize, Args&&... __args)
    {
        for (int i = 0; i < poolSize; i++)
            threads.push_back(std::make_shared<ThreadType>(std::forward<Args>(__args)...));
    }
    virtual ~ThreadPoolBase() = default;

    ThreadPoolBase(const ThreadPoolBase&) = delete;
    ThreadPoolBase& operator=(const ThreadPoolBase&) = delete;

    ThreadPoolBase(ThreadPoolBase&& other) = delete;
    ThreadPoolBase& operator=(ThreadPoolBase&& other) = delete;

    /**
     * Starts all threads in thread pool
     */
    virtual void startThreads()
    {
        for (const auto& thread : threads)
            thread->startThread();
    }

    /**
     * Stops all threads in thread pool
     */
    virtual void stopThreads()
    {
        for (const auto& thread : threads)
            thread->stopThread();
    }

    /**
     * Joins all threads in thread pool
     */
    virtual void joinThreads()
    {
        for (const auto& thread : threads)
            thread->joinThread();
    }

protected:
    std::vector<ThreadTypePtr> threads;
};

#endif //THREADING_THREADPOOLHANDLERBASE_H
