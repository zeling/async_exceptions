//
// Created by zlfeng on 18-1-4.
//

#include "posix.hh"

void *posix_thread::entry_point(void *arg) {
    auto thread_main = reinterpret_cast<std::function<void()> *>(arg);
    thread_main->operator()();
    return nullptr;
}

void posix_thread::join() {
    assert(_valid);
    pthread_join(_pthread, nullptr);
    _valid = false;
}
