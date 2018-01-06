#pragma once

#include <mutex>
#include "posix.hh"
#include <experimental/optional>

template <typename Elem>
class unbounded_mutex_protected_queue {
    std::queue<Elem> _q;
    posix_mutex _m;
public:
    void push(Elem elem) {
        std::lock_guard<posix_mutex> guard(_m);
        _q.push(elem);
    }

    template <typename ...Args>
    void emplace(Args &&...args) {
        std::lock_guard<posix_mutex> guard(_m);
        _q.emplace(std::forward<Args>(args) ...);
    }

    std::experimental::optional<Elem> pop() {
        std::lock_guard<posix_mutex> guard(_m);
        if (_q.empty()) return {};
        std::experimental::optional<Elem> ret(std::move(_q.front()));
        _q.pop();
        return ret;
    }
};

template <typename T>
using umpq = unbounded_mutex_protected_queue<T>;

