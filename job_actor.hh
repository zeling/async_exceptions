#pragma once

#include "actor.hh"
#include "unbounded_mutex_protected_queue.hh"


class job {
    std::unique_ptr<std::function<void ()>> _impl;
public:
    template <typename Func>
    job(Func f) : _impl(std::make_unique<std::function<void ()>>(std::move(f))) {}
    job(const job &) = delete;
    job(job &&other) = default;
    job &operator=(const job &) = delete;
    job &operator=(job &&) = default;
    void run() {
        _impl->operator()();
    }
};

class job_actor : public actor<job, umpq>{
    void respond_to(job j) override {
        j.run();
    }
};
