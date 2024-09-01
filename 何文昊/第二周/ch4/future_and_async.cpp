#include <future>
#include <iostream>

constexpr int find_a_value() noexcept {
  return 1;
}
void do_something() {
  std::cout << "do something!" << std::endl;
}

struct X {
  void foo(int, std::string const&);
  std::string bar(std::string const&);
};

X x;
auto f1 = std::async(&X::foo, &x, 42, "hello");  // 调用 p->foo(42, "hello")，其中 p 是 &x
auto f2 = std::async(&X::bar, x, "goodbye");     // 调用 tmpx.bar("goodbye")，其中 tmpx 是 x 的副本

struct Y {
  double operator()(double);
};

Y y;
auto f3 = std::async(Y(), 3.141);          // 调用 tmpy(3.141)，其中 tmpy 是从 Y() 移动构造的
auto f4 = std::async(std::ref(y), 2.718);  // 调用 y(2.718)
X baz(X&);
auto f5 = std::async (baz, std::ref(x));  // 调用 baz(x)

class move_only {
 public:
  move_only();
  move_only(move_only&&);
  move_only(move_only const&) = delete;
  move_only& operator=(move_only&&);
  move_only& operator=(move_only const&) = delete;
  void operator()();
};

auto f6 = std::async(move_only());  // 调用 tmp()，其中 tmp 是通过 std::move(move_only()) 构造的
auto f7 = std::async(std::launch::async, Y(), 1.2);                                  // 在新线程中运行
auto f8 = std::async(std::launch::deferred, baz, std::ref(x));                       // 在 wait() 或 get() 中运行
auto f9 = std::async(std::launch::deferred | std::launch::async, baz, std::ref(x));  // 实现选择
auto f10 = std::async(baz, std::ref(x));                                              // 实现选择
// f8.wait();                                                                      // 调用延迟函数

int main() {
  std::future<int> the_answer(std::async(find_a_value));
  do_something();
  std::cout << "the answer is: " << the_answer.get() << std::endl;
  return 0;
}
