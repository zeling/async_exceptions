#include <ucontext.h>
#include <stdint.h>
#include <memory>

class lwt_context {
  class stack {
    std::unique_ptr<void, void(*)(void *)> _ptr;
  public:
    explicit stack(size_t size) : _ptr(::malloc(size), ::free) {}
    stack(const stack &) = delete;
    stack(stack &&) = default;
    stack &operator=(const stack &) = delete;
    stack &operator=(stack &&) = default;
    void *base_ptr() noexcept { return _ptr.get(); }
  };
  stack _stack;
  uconext_t _context;

public:
  static constexpr size_t lwt_stack_size = 128 * 1024;
  void switch_in() {
    swapcontext(&g_current_context._context, &_context);
  }

  void switch_out() {
    swapcontext(&_context, &g_current_context._context);
  }
 
  
  lwt_context() : _stack(lwt_stack_size) {
    getcontext(&_context);
    _context.uc_stack.ss_sp = _stack.base_ptr();
    _context.uc_stack.ss_size = lwt_stack_size;
    _context.uc_link = nullptr;
    make_context(&_context, reinterpret_cast<void (*) ()>(&lwt_main), 2, int(this_addr), int(this_addr >> 32));
  }

};

thread_local lwt_context g_current_context;

class lwt_thread {
  ucontext_t _context;
  
  static void lwt_main(int lo, int hi) {
    uintptr_t addr = uint64_t(hi) << 32 | uint64_t(lo);
    reinterpret_cast<lwt_thread *>(addr)->main();
  }

public:
  static constexpr size_t stack_size = 128 * 1024;
  lwt_thread(size_t s): _stack(s) {
    auto this_addr = uintptr_t(this);
    getcontext(&_context);
    _context.uc_stack.ss_sp = _stack.base_ptr();
    _context.uc_stack.ss_size = s;
    _context.uc_link = nullptr;
    makecontext(&_context, reinterpret_cast<void (*) ()>(&lwt_main), 2, int(this_addr), int(this_addr >> 32));
  }

  void switch_in(lwt_thread &orig) {
    swapcontext(&_context, &orig._context);
  }
  void switch_out(lwt_thread &nnew) {
    swapcontext(&nnew._context, &_context);
  }

  lwt_thread(const lwt_thread &) = delete;
  lwt_thread(lwt_thread &&other) = delete;
  lwt_thread &operator= (const stack &) = delete;
  lwt_thread &operator= (stack &&) = delete;
  virtual void main() = 0;
  
};
