//
// Created by konnod on 8/31/18.
//

#ifndef THREADING_GUARDEDDEQUE_H
#define THREADING_GUARDEDDEQUE_H

#include <deque>
#include <mutex>
#include <algorithm>

template <class T>
class GuardedDeque final {
public:
    GuardedDeque() = default;
    ~GuardedDeque() = default;

    GuardedDeque(const GuardedDeque&) = delete;
    GuardedDeque& operator=(const GuardedDeque&) = delete;

    GuardedDeque(GuardedDeque&& other) = delete;
    GuardedDeque& operator=(GuardedDeque&& other) = delete;
//    GuardedDeque(GuardedDeque&& other) noexcept
//    {
//        std::lock_guard<std::mutex> lock(other.mutex);
//        deque = std::move(other.deque);
//    }
//    GuardedDeque& operator=(GuardedDeque&& other) noexcept
//    {
//        std::lock(mutex, other.mutex);
//        std::lock_guard<std::mutex> lock1(mutex, std::adopt_lock);
//        std::lock_guard<std::mutex> lock2(other.mutex, std::adopt_lock);
//        deque = std::move(other.deque);
//        return *this;
//    }

    void pushBack(const T& value) {
        std::lock_guard<std::mutex> lg(mutex);
        deque.push_back(value);
    }

    void pushBack(T&& value) {
        std::lock_guard<std::mutex> lg(mutex);
        deque.push_back(std::move(value));
    }

    template<typename... _Args>
    void emplaceBack(_Args&&... __args) {
        std::lock_guard<std::mutex> lg(mutex);
        deque.emplace_back(std::forward<_Args>(__args)...);
    }

    void pushFront(const T& value) {
        std::lock_guard<std::mutex> lg(mutex);
        deque.push_front(value);
    }

    void pushFront(T&& value) {
        std::lock_guard<std::mutex> lg(mutex);
        deque.push_front(std::move(value));
    }

    template<typename... _Args>
    void emplaceFront(_Args&&... __args) {
        std::lock_guard<std::mutex> lg(mutex);
        deque.emplace_front(std::forward<_Args>(__args)...);
    }

    T getFront() {
        std::lock_guard<std::mutex> lg(mutex);
        if (deque.empty())
            throw std::runtime_error("Deque is empty");
        T value = deque.front();
        deque.pop_front();
        return value;
    }

    T getBack() {
        std::lock_guard<std::mutex> lg(mutex);
        if (deque.empty())
            throw std::runtime_error("Deque is empty");
        T value = deque.back();
        deque.pop_back();
        return value;
    }

    const T& back() const {
        std::lock_guard<std::mutex> lg(mutex);
        if (deque.empty())
            throw std::runtime_error("Deque is empty");
        return deque.back();
    }

    T& back() {
        std::lock_guard<std::mutex> lg(mutex);
        if (deque.empty())
            throw std::runtime_error("Deque is empty");
        return deque.back();
    }

    const T& front() const {
        std::lock_guard<std::mutex> lg(mutex);
        if (deque.empty())
            throw std::runtime_error("Deque is empty");
        return deque.front();
    }

    T& front() {
        std::lock_guard<std::mutex> lg(mutex);
        if (deque.empty())
            throw std::runtime_error("Deque is empty");
        return deque.front();
    }

    void popBack() {
        std::lock_guard<std::mutex> lg(mutex);
        if (deque.empty())
            throw std::runtime_error("Deque is empty");
        deque.pop_back();
    }

    void popFront() {
        std::lock_guard<std::mutex> lg(mutex);
        if (deque.empty())
            throw std::runtime_error("Deque is empty");
        deque.pop_front();
    }

    bool empty() {
        std::lock_guard<std::mutex> lg(mutex);
        return deque.empty();
    }

    size_t size() {
        std::lock_guard<std::mutex> lg(mutex);
        return deque.size();
    }

    void clear() {
        using std::swap;
        std::lock_guard<std::mutex> lg(mutex);
        std::deque<T> empty;
        swap(deque, empty);
    }

    template <class Predicate>
    void removeIf(Predicate p) {
        std::lock_guard<std::mutex> lg(mutex);
        deque.erase(std::remove_if(deque.begin(), deque.end(), p), deque.end());
    }

private:
    std::deque<T> deque;
    std::mutex mutex;
};

#endif //THREADING_GUARDEDDEQUE_H
