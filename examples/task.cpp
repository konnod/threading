//
// Created by konnod on 7/25/19.
//

#include <iostream>
#include "task_thread/TaskThread.h"

int main()
{
    /*
     * Task thread that is waking up every second
     */
    TaskThread::SPtr taskThread = TaskThread::create(std::chrono::milliseconds(1000));

    /*
     * Task that executes every time thread wakes up
     */
    Task::SPtr helloWorldTask = Task::create(
            []() {
                std::cout<< "Hello world!" << std::endl;
            });

    /*
     * Task that executes each 2 seconds
     */
    Task::SPtr slowTask = Task::create(
            []() {
                std::cout<< "Hello world every 2 seconds!" << std::endl;
            },
            std::chrono::milliseconds(2000));
    /*
     * Method addTask() is thread safe, so we even can call it
     * after task thread has been started
     */
    taskThread->addTask(std::move(helloWorldTask));
    taskThread->addTask(std::move(slowTask));

    taskThread->startThread();
    taskThread->joinThread();
    return 0;
}
