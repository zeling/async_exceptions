#include <iostream>
#include "job_actor.hh"
#include <unistd.h>
#include <vector>
#include <sstream>
#include <signal.h>

int fib(int n) {
    return n < 2 ? n : fib(n-1) + fib(n-2);
}

template<int Signal, void(*Func)()>
void install_oneshot_sighandler() {
    static bool handled = false;
    static posix_mutex m;
    struct sigaction sa;
    sa.sa_sigaction = [](int sig, siginfo_t *info, void *p) {
        std::lock_guard<posix_mutex> guard(m);
        if (!handled) {
            handled = true;
            Func();
            ::signal(Signal, SIG_DFL);
        }
    };
    sigfillset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    if (Signal == SIGSEGV) {
        sa.sa_flags |= SA_ONSTACK;
    }
    ::sigaction(Signal, &sa, nullptr);
};

void user1_hdl() {
    std::cout << "handling USER1" << std::endl;
}

void user1_hdl2() {
    std::cout << "handling USER1 in the second handler" << std::endl;
}

void user2_hdl() {
    std::cout << "handling USER2" << std::endl;
}

template <int N>
int *test() {
    static int var;
    return &var;
}

int main() {

    install_oneshot_sighandler<SIGUSR1, user1_hdl>();
    install_oneshot_sighandler<SIGUSR1, user1_hdl2>();
    install_oneshot_sighandler<SIGUSR2, user2_hdl>();

    assert(test<0>() != test<1>());
    assert(test<0>() == test<0>());

    ::kill(::getpid(), SIGUSR1);
    ::kill(::getpid(), SIGUSR2);
//     ::kill(::getpid(), SIGUSR1);

//    struct thread_pool {
//        std::vector<posix_thread> _threads;
//        template <typename Func>
//        thread_pool(size_t n, Func f) : _threads(n, std::forward<Func>(f)) {}
//        ~thread_pool() {
//            for (auto &&thread : _threads) {
//                thread.join();
//            }
//        }
//    };
//
//    std::vector<job_actor> actors(8);
//    std::vector<posix_thread> threads;
//    for (auto &actor : actors) {
//        threads.emplace_back([&actor] { actor.act(); });
//    }
//
//    for (int i = 0; i < 0 + 8 * 3; i++) {
//        actors[i % 8].send_to([i] {
//            std::ostringstream ss;
//            ss << "the result of fib(" << i << ") is " << fib(i) << '\n';
//            std::cout << ss.str();
//        });
//    }
//
//    for (auto &actor : actors) {
//        actor.send_to([&actor] { actor.stop(); });
//    }
//
//    for (auto &&thread : threads) {
//        thread.join();
//    }
//
//    class thread_local_ex : public std::exception {
//    public:
//        std::string msg() const {
//            char buf[256];
//            return { buf, snprintf(buf, 256, "tid: %ld, ptr: %016x", pthread_self(), this) };
//        }
//    };
//
//    job_actor actor;
//    posix_thread bg_thread ([&actor] { actor.act(); });
//    try {
//        throw thread_local_ex();
//    } catch (...) {
//        try {
//            std::rethrow_exception(std::current_exception());
//        } catch (thread_local_ex &ex) {
//            std::cout << "from frontend thread: " << ex.msg() << std::endl;
//        }
//        actor.send_to([&actor, eptr = std::move(std::current_exception())] () {
//            try {
//                std::rethrow_exception(eptr);
//            } catch (thread_local_ex &ex) {
//                std::cout << "from background thread: " << ex.msg() << std::endl;
//            }
//            actor.stop();
//        });
//    }
//    bg_thread.join();
//    return 0;
}
