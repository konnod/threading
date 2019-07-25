//
// Created by konnod on 8/17/18.
//

#ifndef THREADING_GUARDEDMAP_H
#define THREADING_GUARDEDMAP_H

#include <map>
#include <mutex>

template <class K, class V>
class GuardedMap {
private:
    std::map<K, V> map;
    std::mutex m;

public:
    void set(const K &key, const V &value) {
        std::lock_guard<std::mutex> lg(m);
        map[key] = value;
    }

    void set(K &&key, V &&value) {
        std::lock_guard<std::mutex> lg(m);
        map[key] = std::move(value);
    }

    const V *get(const K &key) {
        std::lock_guard<std::mutex> lg(m);
        auto pos = map.find(key);
        if (pos != map.end())
            return &pos->second;
        return nullptr;
    }

    bool empty() {
        std::lock_guard<std::mutex> lg(m);
        return map.empty();
    }

    size_t size() {
        std::lock_guard<std::mutex> lg(m);
        return map.size();
    }

};

#endif //THREADING_GUARDEDMAP_H
