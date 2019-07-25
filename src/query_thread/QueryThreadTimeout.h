//
// Created by konnod on 11/5/18.
//

#ifndef THREADING_QUERYTHREADTIMEOUT_H
#define THREADING_QUERYTHREADTIMEOUT_H

#include <string>
#include <memory>

#include "QueryThreadBase.h"
#include "QueryQueueBase.h"

template<typename _QueryType>
class QueryThreadTimeout : public QueryThreadBase<QueryQueueBase<_QueryType>> {
    typedef QueryThreadBase<QueryQueueBase<_QueryType>> Base;
public:
    typedef typename Base::QueueType QueueType;
    typedef typename Base::QueueTypePtr QueueTypePtr;
    typedef typename Base::QueryType QueryType;
    typedef typename Base::QueryTypePtr QueryTypePtr;
    typedef typename Base::ResultType ResultType;

    explicit QueryThreadTimeout(std::chrono::milliseconds timeoutMs) :
            Base(QueueType::create()),
            timeout(timeoutMs) { }
    ~QueryThreadTimeout() = default;
    QueryThreadTimeout(const QueryThreadTimeout&) = delete;
    QueryThreadTimeout& operator=(const QueryThreadTimeout&) = delete;
    QueryThreadTimeout(QueryThreadTimeout&& other) = delete;
    QueryThreadTimeout& operator=(QueryThreadTimeout&& other) = delete;

private:
    std::chrono::milliseconds timeout;

    void beforeThreadLoop() override {}
    void afterThreadLoop() override {}
    void threadFunction() override
    {
        beforeThreadLoop();
        bool wakenOnSignal = true;
        while (Base::isRunning())
        {
            if (Base::queryQueue->isEmpty())
            {
                    wakenOnSignal = Base::queueCondition->wait_for(timeout,
                                                                WAKE_IF(!Base::queryQueue->isEmpty() ||
                                                                        Base::isStopped()));
            }
            else
            {
                wakenOnSignal = true;
            }

            if (Base::isStopped())
                break;

            if (wakenOnSignal)
            {
                auto query = Base::queryQueue->getQuery();
                onQuery(std::move(query));
            } else
                onTimeout();
        }
        Base::queryQueue->clear();
        afterThreadLoop();
    }

protected:
    /**
     * Overriding function must set result to query.
     */
    virtual void onQuery(QueryTypePtr query) = 0;
    virtual void onTimeout() = 0;
};


#endif //THREADING_QUERYTHREADTIMEOUT_H
