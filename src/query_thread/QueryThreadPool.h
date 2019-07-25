//
// Created by konnod on 11/2/18.
//

#ifndef THREADING_QUERYTHREADPOOLHANDLER_H
#define THREADING_QUERYTHREADPOOLHANDLER_H

#include <memory>

#include "../ThreadPoolBase.h"
#include "../utils/Condition.h"

template<typename _QueryThreadType>
class QueryThreadPool : public ThreadPoolBase<_QueryThreadType> {
    typedef ThreadPoolBase<_QueryThreadType> Base;
public:
    typedef typename Base::ThreadType QueryThreadType;
    typedef typename Base::ThreadTypePtr ThreadTypePtr;
    typedef typename QueryThreadType::QueueType QueueType;
    typedef typename QueryThreadType::QueueTypePtr QueueTypePtr;
    typedef typename QueryThreadType::QueryType QueryType;
    typedef typename QueryThreadType::QueryTypePtr QueryTypePtr;
    typedef typename QueryThreadType::ResultType ResultType;
    typedef typename QueryThreadType::ResultTypePtr ResultTypePtr;

    /**
     * Derived class constructor must create number of custom type threads,
     * pass queue to their constructors and push them to vector
     */
    template<typename... Args>
    explicit QueryThreadPool(unsigned int poolSize, QueueTypePtr queue, Args&&... args)
            : Base(poolSize, queue, std::forward<Args>(args)...)
            , queryQueue(queue)
            , queueCondition(queryQueue->getHasQueryCondition()) {}

    virtual ~QueryThreadPool() = default;

    QueryThreadPool(const QueryThreadPool&) = delete;
    QueryThreadPool& operator=(const QueryThreadPool&) = delete;

    QueryThreadPool(QueryThreadPool&& other) = delete;
    QueryThreadPool& operator=(QueryThreadPool&& other) = delete;

    void stopThreads() override
    {
        Base::stopThreads();
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
    /**
     * Query queue connected with thread to put queries to
     */
    QueueTypePtr queryQueue;
    Condition::SPtr queueCondition;
};


#endif //THREADING_QUERYTHREADPOOLHANDLER_H
