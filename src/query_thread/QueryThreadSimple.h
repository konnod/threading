//
// Created by konnod on 11/5/18.
//

#ifndef THREADING_QUERYTHREADSIMPLE_H
#define THREADING_QUERYTHREADSIMPLE_H

#include <string>
#include <memory>

#include "QueryThreadBase.h"
#include "QueryQueueBase.h"

template<typename _QueryType>
class QueryThreadSimple : public QueryThreadBase<QueryQueueBase<_QueryType>> {
    typedef QueryThreadBase<QueryQueueBase<_QueryType>> Base;
public :
    typedef typename Base::QueueType QueueType;
    typedef typename Base::QueueTypePtr QueueTypePtr;
    typedef typename Base::QueryType QueryType;
    typedef typename Base::QueryTypePtr QueryTypePtr;
    typedef typename Base::ResultType ResultType;
    typedef typename Base::ResultTypePtr ResultTypePtr;

    QueryThreadSimple() :
            Base(QueueType::create()) { }
    ~QueryThreadSimple() = default;
    QueryThreadSimple(const QueryThreadSimple&) = delete;
    QueryThreadSimple& operator=(const QueryThreadSimple&) = delete;
    QueryThreadSimple(QueryThreadSimple&& other) = delete;
    QueryThreadSimple& operator=(QueryThreadSimple&& other) = delete;

private:
    void beforeThreadLoop() override {}
    void afterThreadLoop() override {}
    void threadFunction() override
    {
        beforeThreadLoop();
        while (Base::isRunning())
        {
            if (Base::queryQueue->isEmpty())
            {
                Base::queueCondition->wait(
                            WAKE_IF(!Base::queryQueue->isEmpty() ||
                                      Base::isStopped()));
            }

            if (Base::isStopped())
                break;

            auto query = Base::queryQueue->getQuery();
            onQuery(std::move(query));
        }
        Base::queryQueue->clear();
        afterThreadLoop();
    }

protected:
    virtual void onQuery(QueryTypePtr query) = 0;
};


#endif //THREADING_QUERYTHREADSIMPLE_H
