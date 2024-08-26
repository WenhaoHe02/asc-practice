#include <mutex>
#include <stdexcept>
#include <thread>
class hierarchical_mutex {
 public:
  explicit hierarchical_mutex(unsigned long value) : hierarchy_value(value), previous_hierarchy_value(0) {}
  void lock() {
    check_for_hierarchy_violation();
    internal_mutex.lock();
    update_hierarchy_value();
  }
  void unlock() {
    if (this_thread_hierarchy_value != hierarchy_value) {
      throw std::logic_error("mutex hierarchy violated");
    }
    this_thread_hierarchy_value = previous_hierarchy_value;
    internal_mutex.unlock();
  }
  bool try_lock() {
    check_for_hierarchy_violation();
    if (!internal_mutex.try_lock()) {
      return false;
    }
    update_hierarchy_value();
    return true;
  }

 private:
  std::mutex internal_mutex;
  unsigned long const hierarchy_value;
  unsigned long previous_hierarchy_value;
  static thread_local unsigned long this_thread_hierarchy_value;
  void check_for_hierarchy_violation() const {
    if (this_thread_hierarchy_value <= hierarchy_value) {
      throw std::logic_error("mutex hierarchical violated");
    }
  }
  void update_hierarchy_value() {
    previous_hierarchy_value    = this_thread_hierarchy_value;
    this_thread_hierarchy_value = hierarchy_value;
  }
};
hierarchical_mutex high_level_mutex(10000);
hierarchical_mutex low_level_mutex(5000);
hierarchical_mutex other_mutex(6000);
int do_low_level_stuff();

int low_level_func() {
  std::lock_guard<hierarchical_mutex> lk(low_level_mutex);
  return do_low_level_stuff();
}

void high_level_stuff(int some_param);

void high_level_func() {
  std::lock_guard<hierarchical_mutex> lk(high_level_mutex);
  high_level_stuff(low_level_func());
}

void thread_a() {
  high_level_func();
}

void do_other_stuff();

void other_stuff() {
  high_level_func();
  do_other_stuff();
}

void thread_b() {
  std::lock_guard<hierarchical_mutex> lk(other_mutex);  //错误的，含有value为6000锁的线程不能锁上value为10000的
  other_stuff();
}
