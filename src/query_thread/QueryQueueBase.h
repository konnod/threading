//
// Created by konnod on 10/19/18.
//

#ifndef THREADING_QUERYQUEUEBASE_H
#define THREADING_QUERYQUEUEBASE_H

#include <memory>
#include <atomic>

#include "utils/Condition.h"
#include "utils/GuardedDeque.h"

/**
 *
 * @tparam _QueryType The type of query that queue will hold. Just type, not shared_ptr on type.
 * QueryQueue is neither copyable nor movable.
 */
template<typename _QueryType>
class QueryQueueBase {
public:
    /*
     * Queue holds shared_ptr of passed type within itself
     */
    typedef _QueryType QueryType;
    typedef std::shared_ptr<QueryType> QueryTypePtr;
    typedef typename QueryType::ResultType ResultType;
    typedef typename QueryType::ResultTypePtr ResultTypePtr;

    QueryQueueBase() : hasQueryCondition(Condition::create()) { }
    virtual ~QueryQueueBase() {
        clear();
    }
    QueryQueueBase(const QueryQueueBase&) = delete;
    QueryQueueBase& operator=(const QueryQueueBase&) = delete;
    QueryQueueBase(QueryQueueBase&& other) = delete;
    QueryQueueBase& operator=(QueryQueueBase&& other) = delete;

    virtual void pushQuery(const QueryTypePtr &query)
    {
        queryDeque.pushBack(query);
        hasQueryCondition->notify_one();
    }

    virtual void pushQuery(QueryTypePtr &&query)
    {
        queryDeque.pushBack(std::move(query));
        hasQueryCondition->notify_one();
    }

    template<typename... _Args>
    void emplaceQuery(_Args&&... __args)
    {
        queryDeque.emplaceBack(std::make_shared<QueryType>(std::forward<_Args>(__args)...));
        hasQueryCondition->notify_one();
    }

    /**
     * Removes query from queue and returns it.
     * If queue is empty it throws std::runtime_error
     */
    virtual QueryTypePtr getQuery()
    { return queryDeque.getFront(); }

    virtual const QueryTypePtr& frontQuery()
    { return queryDeque.front(); }

    virtual void popQuery()
    { queryDeque.popFront(); }

    Condition::SPtr
    getHasQueryCondition() const
    { return hasQueryCondition; }

    bool isEmpty()
    { return queryDeque.empty(); }

    size_t size()
    { return queryDeque.size(); }

    /**
     * Clears the queue and sets result for all queries
     */
    void clear()
    {
        while (!queryDeque.empty()) {
            QueryTypePtr p = queryDeque.getFront();
            p->setResult();
            p->invalidate();
        }
    }

    template <class Predicate>
    void removeIf(Predicate p)
    { queryDeque.removeIf(p); }

protected:
    Condition::SPtr hasQueryCondition;
    GuardedDeque<QueryTypePtr> queryDeque;
};

#endif //THREADING_QUERYQUEUEBASE_H
