#include <iostream>
#include <string>
#include <thread>
using namespace std;
//向线程函数传递参数：在初始化thread时传递，参数会先复制到内部储存中，然后以右值形式传递,所以要注意语义的转换
auto do_something(int num, string const& str) -> void {
  cout << format("{}{}\n", num, str);
}
// auto oops(int num) -> void {
//   char buffer[200];
//   sprintf(buffer, "%i", num);
//   std::thread t(do_something, 3, buffer);  //buffer传参是要转换成string，可能在转换前oops函数就退出，导致未定义行为
//   t.detach();
// }
auto not_oops(int num) -> void {
  char buffer[200];
  sprintf(buffer, "%i", num);
  std::thread t(do_something, 3, std::string(buffer));  //正确的写法
  t.detach();
}
/*
#include <functional>
void update_data_for_widget(widget_id w, widget_data& data) {
  
}
void oops_again(widget_id w) {
  widget_data data;
  std::thread t(update_data_for_widget, w, data);// 传递的还是右值，对于引用类型不能编译
  display_status();
  t.join();
  process_widget_data(data);
}
void not_oops_again(widget_id w) {
  widget_data data;
  std::thread t(update_data_for_widget, w, std::ref(data));
  display_status();
  t.join();
  process_widget_data(data);
}
*/

/*thread传参和std::bind操作定义基于相同的机制,所以有
在新线程上调用 my_x.do_lengthy_work()，因为提供了 my_x 的地址作为对象指针。
还可以为这种成员函数调用提供参数：std::thread 构造函数的第三个参数将成为成员函数的第一个参数，依此类推。
*/
class X {
 public:
  void do_lengthy_work();
};
void f() {
  X my_x;
  std::thread t(&X::do_lengthy_work, &my_x);
}

auto main() -> int {
  // oops(1);
  not_oops(2);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return 0;
}