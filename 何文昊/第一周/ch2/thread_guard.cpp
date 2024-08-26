// 在发生异常时，必须确保退出时所有其他线程都得到妥善的资源管理，根据RAII，可以利用一个Thread_Guard类设计解决
#include <exception>
#include <iostream>
#include <stdexcept>
#include <thread>
using namespace std;
auto do_something() -> void {
  cout << format("Hello,mutithread!\n");
}
class Thread_Guard {
 public:
  explicit Thread_Guard(thread& t) : t_(t) {}
  ~Thread_Guard() {
    if (t_.joinable()) {
      t_.join();
      cout << format("finish join!\n");
    }
  }
  Thread_Guard(thread const& t)            = delete;
  Thread_Guard& operator=(thread const& t) = delete;
  Thread_Guard(thread&& t)                 = delete;
  Thread_Guard& operator=(thread&& t)      = delete;

 private:
  thread& t_;
};
auto main() -> int {
  try {
    thread t1(do_something);
    Thread_Guard guard_of_t1(t1);
    throw runtime_error("this is an exception");
    //在发生异常后，会最先调用Thread_Guard的析构函数，然后才析构t1，非常巧妙
    t1.join();  //不会调用
  } catch (exception& e) {
    // 可以选择在这里join，但可能有遗漏
    cout << e.what();
  }

  return 0;
}