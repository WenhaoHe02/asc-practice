#include <mutex>
void prepare_data();
void do_something();
std::unique_lock<std::mutex> get_lock() {
  extern std::mutex some_mutex;
  std::unique_lock<std::mutex> lk(some_mutex);
  prepare_data();
  return lk; //编译器自动优化
}

void process_data() {
  std::unique_lock<std::mutex> lk(get_lock());
  do_something();
}
