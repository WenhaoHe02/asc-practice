#include <iostream>
#include <thread>
using namespace std;
auto do_something() -> void {
  cout << format("Hello,mutithread!\n");
}
class Task {
 public:
  void operator()() {
    cout << "show in a class ";
    do_something();
  }
};  // 用类提交任务
auto main() -> int {
  cout << format("this is main\n");
  thread t1(do_something);
  thread t2{Task()};  //在传无名类时避免恶心人的初始化解析
  thread t3([] { do_something(); });
  t1.join();  // 主线程等待t1完成
  t2.join();
  t3.detach();  // t3 在后台完成 注意:这种写法要考虑其中变量的声明周期有没有依赖于其他线程！
  //特别是对临时变量的引用之类的不要用
  return 0;
}