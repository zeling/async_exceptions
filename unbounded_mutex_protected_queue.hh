#pragma once

#include <mutex>
#include "posix.hh"

template <typename Elem>
class unbounded_mutex_protected_queue {
    std::queue<Elem> _q;
    posix_mutex _m;
public:
    Elem &&front() {
        std::lock_guard<posix_mutex> guard(_m);
        return std::move(_q.front());
    }

    void push(Elem elem) {
        std::lock_guard<posix_mutex> guard(_m);
        _q.push(elem);
    }

    template <typename ...Args>
    void emplace(Args &&...args) {
        std::lock_guard<posix_mutex> guard(_m);
        _q.emplace(std::forward<Args>(args) ...);
    }

    void pop() {
        std::lock_guard<posix_mutex> guard(_m);
        _q.pop();
    }

    bool empty() {
        std::lock_guard<posix_mutex> guard(_m);
        return _q.empty();
    }
};

template <typename T>
using umpq = unbounded_mutex_protected_queue<T>;

