#include <iostream>
#include "job_actor.hh"
#include <unistd.h>

int main() {
    class thread_local_ex : public std::exception {
    public:
        std::string msg() const {
            char buf[256];
            return { buf, snprintf(buf, 256, "tid: %ld, ptr: %016x", pthread_self(), this) };
        }
    };

    job_actor actor;
    posix_thread bg_thread ([&actor] { actor.act(); });
    try {
        throw thread_local_ex();
    } catch (...) {
        try {
            std::rethrow_exception(std::current_exception());
        } catch (thread_local_ex &ex) {
            std::cout << "from frontend thread: " << ex.msg() << std::endl;
        }
        actor.send_to([&actor, eptr = std::move(std::current_exception())] () {
            try {
                std::rethrow_exception(eptr);
            } catch (thread_local_ex &ex) {
                std::cout << "from background thread: " << ex.msg() << std::endl;
            }
            actor.stop();
        });
    }
    bg_thread.join();
    return 0;
}