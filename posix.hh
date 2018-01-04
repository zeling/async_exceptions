#pragma once

#include <pthread.h>
#include <queue>
#include <bits/unique_ptr.h>
#include <functional>
#include <assert.h>

class posix_thread;
class posix_mutex;

class posix_thread {
    pthread_t _pthread;
    std::unique_ptr<std::function<void ()>> _func;
    bool _valid = true;
    static void *entry_point(void *);
public:
    posix_thread(const posix_thread &) = delete;
    posix_thread(posix_thread &&other) noexcept
            : _pthread(other._pthread),
              _func(std::move(other._func)),
              _valid(other._valid)
    {
        other._valid = false;
    }

    posix_thread &operator= (const posix_thread &) = delete;
    posix_thread &operator= (posix_thread &&rhs) noexcept {
        if (this != &rhs) {
            delete this;
            new (this) posix_thread(std::move(rhs));
        }
        return *this;
    }
    ~posix_thread() { assert(!_valid); }

    template <typename Func>
    posix_thread(Func f): _func(std::make_unique<std::function<void ()>>(std::move(f))) {
        pthread_attr_t attr {};
        pthread_attr_init(&attr);
        pthread_create(&_pthread, &attr, &posix_thread::entry_point, _func.get());
    }

    void join();
};

class posix_mutex {
    pthread_mutex_t _pthread_mutex;
public:
    posix_mutex() {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutex_init(&_pthread_mutex, &attr);
    }
    ~posix_mutex() {
        pthread_mutex_destroy(&_pthread_mutex);
    }

    void lock() {
        pthread_mutex_lock(&_pthread_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&_pthread_mutex);
    }
};

