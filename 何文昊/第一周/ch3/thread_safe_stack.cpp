#include <exception>
#include <memory>
#include <mutex>
#include <stack>
struct empty_stack : std::exception {
  char const* what() const throw();
};
template <typename T>
class threadsafe_stack {
 private:
  std::stack<T> data;
  mutable std::mutex m;

 public:
  threadsafe_stack() = default;
  threadsafe_stack(threadsafe_stack const& other) {
    std::lock_guard<std::mutex> lock(other.m);
    data = other.data;
  }
  threadsafe_stack& operator=(threadsafe_stack const&) = delete;
  void push(T new_value) {
    std::lock_guard<std::mutex> lock(m);
    data.push(std::move(new_value));
  }
  std::shared_ptr<T> pop() {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) {
      throw empty_stack();
    }
    std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
    data.pop();
    return res;
  }
  void pop(T& value) {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) {
      throw empty_stack();
    }
    value = data.top();
    data.pop();
  }
  bool empty() const {
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
  }
};