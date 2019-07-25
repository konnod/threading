//
// Created by konnod on 11/1/18.
//

#ifndef THREADING_QUERYBASE_H
#define THREADING_QUERYBASE_H

#include <future>
#include <memory>
#include <chrono>
#include <atomic>

/**
 * @class QueryBase
 * QueryBase is designed to be the base class for query to be put into query queue
 * and to be processed by the query thread.
 * QueryBase is not copyable and not movable and so are derived classes.
 * Use shared_ptr on this class.
 * @tparam _ResultType The type of query's returned data
 */
template<typename _ResultType>
class QueryBase {
public:
    typedef _ResultType ResultType;

    QueryBase() : future(promise.get_future()), valid(true) { }
    virtual ~QueryBase() = default;

    QueryBase(const QueryBase&) = delete;
    QueryBase& operator=(const QueryBase&) = delete;

    QueryBase(QueryBase&& other) = delete;
    QueryBase& operator=(QueryBase&& other) = delete;

    /**
     * @brief Gets the result
     *
     * Returns the result immediately if result is set
     * or waits until result is set and then returns it
     * @return Result value copy
     */
    ResultType getResult()
    {
        return future.get();
    }

    /**
     * @brief Waits for result to be set for specified amount of time
     * @param timeout time to wait
     * @return true if result is set
     */
    bool waitForResult(std::chrono::milliseconds timeout)
    {
        return future.wait_for(timeout) == std::future_status::ready;
    }

    /**
     * @brief Sets the result
     */
    void setResult(const ResultType& res)
    {
        promise.set_value(res);
    }

    /**
     * @brief Sets the result
     */
    void setResult(ResultType&& res)
    {
        promise.set_value(std::move(res));
    }

    /**
     *
     * @return true if someone else is still waiting for query to be processed
     */
    bool isValid()
    {
        return valid.load(std::memory_order_relaxed);
    }

    /**
     * @brief Call it to set that query does not need to be processed anymore
     */
    void invalidate()
    {
        valid = false;
    }

private:
    std::promise<ResultType> promise;
    std::future<ResultType> future;

    // Indicates if the created thread still waits for query to be processed
    std::atomic_bool valid;
};

/**
 * Explicit specialization for void result type
 */
template<>
class QueryBase<void> {
public:
    typedef void ResultType;

    QueryBase() : future(promise.get_future()), valid(true) { }
    virtual ~QueryBase() = default;

    QueryBase(const QueryBase&) = delete;
    QueryBase& operator=(const QueryBase&) = delete;
    QueryBase(QueryBase&& other) = delete;
    QueryBase& operator=(QueryBase&& other) = delete;

    /**
     * @brief Gets the result
     *
     * Returns the result immediately if result is set
     * or waits until result is set and then returns it
     * @return Result value copy
     */
    ResultType getResult()
    {
        return future.get();
    }

    /**
     * @brief Waits for result to be set for specified amount of time
     * @param timeout time to wait
     * @return true if result is set
     */
    bool waitForResult(std::chrono::milliseconds timeout)
    {
        return future.wait_for(timeout) == std::future_status::ready;
    }

    /**
     * @brief Sets the result
     */
    void setResult()
    {
        promise.set_value();
    }

    /**
     *
     * @return true if someone else is still waiting for query to be processed
     */
    bool isValid()
    {
        return valid.load(std::memory_order_relaxed);
    }

    /**
     * @brief Call it to set that query does not need to be processed anymore
     */
    void invalidate()
    {
        valid = false;
    }

private:
    std::promise<ResultType> promise;
    std::future<ResultType> future;

    // Indicates if the created thread still waits for query to be processed
    std::atomic_bool valid;
};

#endif //THREADING_QUERYBASE_H
