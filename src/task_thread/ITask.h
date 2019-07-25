//
// Created by konnod on 3/1/19.
//

#ifndef THREADING_ITASK_H
#define THREADING_ITASK_H

class ITask {
public:
    /**
     * Checks whether this task must be executed
     * @return true if it is time to execute this task, false otherwise
     */
    virtual bool isTimeToExecute() const = 0;

    /**
     * Actual task work must be done here
     */
    virtual void execute() = 0;
};

#endif //THREADING_ITASK_H
