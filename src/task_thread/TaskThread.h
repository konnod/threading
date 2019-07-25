//
// Created by konnod on 11/23/18.
//

#ifndef THREADING_TASKTHREAD_H
#define THREADING_TASKTHREAD_H

#include <chrono>
#include <list>
#include <mutex>

#include "Task.h"
#include "ThreadBase.h"
#include "utils/SPtrFactoryBase.h"

/**
 * @class TaskThread
 * @brief Thread that executes tasks with certain frequency
 */
class TaskThread : public ThreadBase, public SPtrFactoryBase<TaskThread> {
public:
    /**
     * @brief TaskThread constructor
     * @param wakeupPeriod how often to wake up to execute tasks
     */
    explicit TaskThread(std::chrono::milliseconds wakeupPeriod) :
            wakeupPeriod(wakeupPeriod) { }

    void startThread() override
    {
        nextWakeup = std::chrono::steady_clock::now() + wakeupPeriod;
        ThreadBase::startThread();
    }

    /**
     * @brief Adds task to execute
     * @param task Task to execute
     */
    void addTask(std::shared_ptr<ITask>&& task)
    {
        std::lock_guard<std::mutex> lock(tasksListMutex);
        taskList.push_back(std::move(task));
    }

    /**
     * @brief Adds task to execute
     * @param task Task to execute
     */
    void addTask(const std::shared_ptr<ITask>& task)
    {
        std::lock_guard<std::mutex> lock(tasksListMutex);
        taskList.push_back(task);
    }

private:
    void threadIteration() override;

    std::chrono::milliseconds wakeupPeriod;
    std::chrono::steady_clock::time_point nextWakeup;

    /**
     * Mutex to make tasksList thread-safe
     */
    std::mutex tasksListMutex;
    /// The list of tasks
    std::list<std::shared_ptr<ITask>> taskList;

    /**
     * @brief Runs all tasks
     */
    void runTasks();
};

void inline TaskThread::threadIteration()
{
    std::this_thread::sleep_for(wakeupPeriod);

    runTasks();

    nextWakeup = std::chrono::steady_clock::now() + wakeupPeriod;
}


void inline TaskThread::runTasks()
{
    std::lock_guard<std::mutex> lock(tasksListMutex);

    for (auto& task : taskList)
    {
        if (task->isTimeToExecute())
        {
            task->execute();
        }
    }
}

#endif //THREADING_TASKTHREAD_H
