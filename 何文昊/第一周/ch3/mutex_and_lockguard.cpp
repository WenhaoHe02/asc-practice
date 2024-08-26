#include <iostream>
#include <list>
#include <mutex>
#include <thread>
std::mutex mut;
std::list<int> lis;
auto add_to_list(int value) -> void {
  //lock_guard可自动管理锁的开闭
  std::lock_guard /*<std::mutex>*/ guard(mut);  //c++17后可省略模板参数
  lis.push_back(value);
}
auto add_to_list(int value, int type) -> void {
  //scoped_lock 用于多锁管理，可避免死锁
  std::scoped_lock /*<std::mutex>*/ scoped_lock_(mut);  //c++17后可省略模板参数
  lis.push_back(value);
}
auto list_contains(int value_to_find) -> bool {
  std::lock_guard<std::mutex> guard(mut);
  return std::find(lis.begin(), lis.end(), value_to_find) != lis.end();
}
auto main() -> int {
  std::thread t1([] { add_to_list(1); });
  std::thread t2([] { add_to_list(2, 1); });
  std::thread t3([] { std::cout << static_cast<bool>(list_contains(1)) << std::endl; });
  t1.join();
  t2.join();
  t3.join();
  for (auto const& ele : lis) {
    std::cout << ele << std::endl;
  }
  return 0;
}