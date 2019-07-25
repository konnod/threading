//
// Created by konnod on 1/10/19.
//

#ifndef THREADING_TASK_H
#define THREADING_TASK_H

#include <chrono>
#include <functional>
#include "ITask.h"
#include "utils/SPtrFactoryBase.h"

/**
 * @class Task
 * @brief Task to be executed by TaskThread
 *
 * Repetition period resolution in milliseconds
 */
class Task : public ITask, public SPtrFactoryBase<Task> {
public:
    /**
     * @brief Task constructor
     * @param taskFunction task to execute
     *        User can use std::bind to pass custom parameters to taskFunction
     * @param repetitionPeriod how often to run this task,
     *        if 0 - task will be performed on each thread wakeup
     */
    explicit Task(std::function<void()> taskFunction,
         std::chrono::milliseconds repetitionPeriod = std::chrono::milliseconds(0))
        : repetitionPeriod(repetitionPeriod)
        , nextInvocation(std::chrono::steady_clock::now() + repetitionPeriod)
        , taskFunction(std::move(taskFunction)) { }

    virtual ~Task() = default;

    /**
     * Checks whether if repetition period had passed after last task execution
     * @return true it is time to execute this task
     */
    bool isTimeToExecute() const override {
        return std::chrono::steady_clock::now() >= nextInvocation;
    }

    /**
     * @brief Executes task and sets next invocation time according to repetition period
     */
    void execute() override {
        taskFunction();
        nextInvocation = std::chrono::steady_clock::now() + repetitionPeriod;
    }

protected:
    std::chrono::milliseconds repetitionPeriod;
    std::chrono::steady_clock::time_point nextInvocation;
    std::function<void()> taskFunction;
};

#endif //THREADING_TASK_H
