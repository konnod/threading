//
// Created by konnod on 11/5/18.
//

#ifndef THREADING_QUERYTHREADPOOLWORKER_H
#define THREADING_QUERYTHREADPOOLWORKER_H

#include <string>
#include <memory>

#include "QueryThreadBase.h"
#include "QueryQueueBase.h"

template<typename _QueryType>
class QueryThreadPoolThread : public QueryThreadBase<QueryQueueBase<_QueryType>> {
    typedef QueryThreadBase<QueryQueueBase<_QueryType>> Base;
public :
    typedef typename Base::QueueType QueueType;
    typedef typename Base::QueueTypePtr QueueTypePtr;
    typedef typename Base::QueryType QueryType;
    typedef typename Base::QueryTypePtr QueryTypePtr;
    typedef typename Base::ResultType ResultType;
    typedef typename Base::ResultTypePtr ResultTypePtr;

    explicit QueryThreadPoolThread(const QueueTypePtr& queue) :
            Base(queue) { }
    ~QueryThreadPoolThread() = default;
    QueryThreadPoolThread(const QueryThreadPoolThread&) = delete;
    QueryThreadPoolThread& operator=(const QueryThreadPoolThread&) = delete;
    QueryThreadPoolThread(QueryThreadPoolThread&& other) = delete;
    QueryThreadPoolThread& operator=(QueryThreadPoolThread&& other) = delete;

private:
    void beforeThreadLoop() override {}
    void afterThreadLoop() override {}
    void threadFunction() override
    {
        beforeThreadLoop();
        while (Base::isRunning())
        {
            QueryTypePtr query;
            if (Base::queryQueue->isEmpty())
            {
                Base::queueCondition->wait(
                            WAKE_IF(!Base::queryQueue->isEmpty() ||
                                    Base::isStopped()));
            }

            if (Base::isStopped())
                break;

            try {
                query = Base::queryQueue->getQuery();
            } catch (std::runtime_error& e) {
                // Another thread has taken the query
                continue;
            }

            onQuery(std::move(query));
        }
        Base::queryQueue->clear();
        afterThreadLoop();
    }

protected:
    virtual void onQuery(QueryTypePtr query) = 0;
};


#endif //THREADING_QUERYTHREADPOOLWORKER_H
