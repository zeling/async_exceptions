#pragma once

#include <queue>
#include <atomic>
#include <assert.h>

template <typename Msg, template<typename Elem> class Queue>
class actor {
public:
    using message_type = Msg;
    using queue_type = Queue<Msg>;
private:
    queue_type _q;
    std::atomic<bool> _stop = false;
public:
    actor(): _q(), _stop(false) {}
    virtual ~actor() {
        assert(_stop.load(std::memory_order_acquire));
    }
    virtual void respond_to(message_type msg) = 0;
    template <typename ...Args>
    void send_to(Args &&...args) {
        _q.emplace(std::forward<Args>(args)...);
    }
    void act() {
        while(!_stop.load(std::memory_order_acquire)) {
            if (!_q.empty()) {
                respond_to(std::move(_q.front()));
                _q.pop();
            }
        }
    }
    void stop() {
        _stop.store(true, std::memory_order_release);
    }
};
