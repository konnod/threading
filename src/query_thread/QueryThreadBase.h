//
// Created by konnod on 10/22/18.
//

#ifndef THREADING_QUERYTHREADBASE_H
#define THREADING_QUERYTHREADBASE_H

#include <string>
#include <memory>

#include "utils/Condition.h"
#include "../ThreadBase.h"

 /**
  * QueryThreadBase is neither copyable nor movable.
  * @sa QueryQueue
  */
template<typename _QueueType>
class QueryThreadBase : public ThreadBase {
public:
    typedef _QueueType QueueType;
    typedef typename std::shared_ptr<QueueType> QueueTypePtr;
    typedef typename QueueType::QueryType QueryType;
    typedef typename QueueType::QueryTypePtr QueryTypePtr;
    typedef typename QueryType::ResultType ResultType;
    typedef typename QueryType::ResultTypePtr ResultTypePtr;

    explicit QueryThreadBase(const QueueTypePtr& queue) :
            queryQueue(queue),
            queueCondition(queryQueue->getHasQueryCondition()) { }

    ~QueryThreadBase() override
    {
        stopThread();
        joinThread();
    };

    QueryThreadBase(const QueryThreadBase&) = delete;
    QueryThreadBase& operator=(const QueryThreadBase&) = delete;

    QueryThreadBase(QueryThreadBase&& other) = delete;
    QueryThreadBase& operator=(QueryThreadBase&& other) = delete;

    void stopThread() override
    {
        ThreadBase::stopThread();
        queueCondition->notify_all();
    }

    QueueTypePtr getQueue() const
    {
        return queryQueue;
    }

    unsigned int getQueueSize() {
        return queryQueue->size();
    };

    Condition::SPtr getHasQueryCondition() const {
        return queueCondition;
    }

    void putQuery(QueryTypePtr&& query) {
        queryQueue->pushQuery(std::move(query));
    }

    void putQuery(const QueryTypePtr& query) {
        queryQueue->pushQuery(query);
    }

    ResultTypePtr putQueryAndGetResult(const QueryTypePtr &query) {
        queryQueue->pushQuery(query);
        return query->getResult();
    }

    template<typename... _Args>
    void emplaceQuery(_Args&&... __args) {
        queryQueue->emplaceQuery(std::forward<_Args>(__args)...);
    }

    template<typename... _Args>
    ResultTypePtr emplaceQueryAndGetResult(_Args&&... __args) {
        QueryTypePtr query = std::make_shared<QueryType>(std::forward<_Args>(__args)...);
        queryQueue->pushQuery(query);
        return query->getResult();
    }

protected:
    QueueTypePtr queryQueue;
    Condition::SPtr queueCondition;
};

#endif //THREADING_QUERYTHREADBASE_H
