#include <iostream>
#include <thread>
using namespace std;
auto some_function() -> void {
  cout << "some_function\n";
}
auto some_other_function() -> void {
  cout << "some_other_function\n";
}
auto f() -> thread {
  return thread(some_function);
}
auto g() -> thread {
  std::thread t(some_other_function);
  return t;
}
//由于移动的特性，可以写出thread_guard的升级版
class scoped_thread {
  std::thread t_;

 public:
  explicit scoped_thread(std::thread t) : t_(std::move(t)) {
    if (!t.joinable()) {
      throw std::logic_error("No thread");
    }
  }
  ~scoped_thread() {
    t_.join();
  }
  scoped_thread(scoped_thread const&)            = delete;
  scoped_thread& operator=(scoped_thread const&) = delete;
};
void f2() {
  std::vector<std::thread> threads;
  for (unsigned i = 0; i < 20; ++i) {
    threads.emplace_back(some_function, i);
  }
  for (auto& entry : threads) {
    entry.join();
  }
}
auto main() -> int {
  thread t1(some_function);
  thread t2(move(t1));
  t1 = thread(some_other_function);
  thread t3(move(t2));
  t1 = move(t3);
}
