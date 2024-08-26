#include <mutex>
#include <thread>
class big_object {
 public:
  big_object(int num) : num_(num) {}

 private:
  int num_;
};
auto swap(big_object& lhs, big_object& rhs) -> void;
class X {
 private:
  big_object detail_;
  std::mutex mut;

 public:
  X(big_object const& detail) : detail_(detail) {}
  // friend void swap(X& lhs, X& rhs) {
  //   if (&lhs == &rhs) {
  //     return;
  //   }
  //   std::lock(lhs.mut, rhs.mut);
  //   std::lock_guard<std::mutex> lock_a(lhs.mut, std::adopt_lock);
  //   std::lock_guard<std::mutex> lock_b(rhs.mut, std::adopt_lock);
  //   swap(lhs.detail_, rhs.detail_);
  // }
  friend void swap(X& lhs, X& rhs) {
    if (&lhs == &rhs) {
      return;
    }
    std::scoped_lock guard(lhs.mut, rhs.mut);
    swap(lhs.detail_, rhs.detail_);
  }
};