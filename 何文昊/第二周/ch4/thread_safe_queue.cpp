// #include <condition_variable>
// #include <memory>
// #include <mutex>
// #include <queue>
// template <typename T>
// class threadsafe_queue {
//  private:
//   std::queue<T> data_queue;
//   mutable std::mutex mut;
//   std::condition_variable data_cond;

//  public:
//   threadsafe_queue()                                         = default;
//   threadsafe_queue(threadsafe_queue const& other)            = delete;
//   threadsafe_queue& operator=(threadsafe_queue const& other) = delete;
//   ~threadsafe_queue()                                        = delete;

//   void push(T new_value) {
//     std::lock_guard<std::mutex> lk(mut);
//     data_queue.push(std::move(new_value));
//     data_cond.notify_one();
//   }

//   bool try_pop(T& value) {
//     std::lock_guard<std::mutex> lk(mut);
//     if (data_queue.empty()) {
//       return false;
//     }
//     value = std::move(data_queue.front());
//     data_queue.pop();
//     return true;
//   }

//   std::shared_ptr<T> try_pop() {
//     std::lock_guard<std::mutex> lk(mut);
//     if (data_queue.empty()) {
//       return std::shared_ptr<T>();
//     }
//     std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
//     data_queue.pop();
//     return res;
//   }

//   void wait_and_pop(T& value) {
//     std::unique_lock<std::mutex> lk(mut);
//     data_cond.wait(lk, [this] { return !data_queue.empty(); });
//     value = std::move(data_queue.front());
//     data_queue.pop();
//   }

//   std::shared_ptr<T> wait_and_pop() {
//     std::unique_lock<std::mutex> lk(mut);
//     data_cond.wait(lk, [this] { return !data_queue.empty(); });
//     std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
//     data_queue.pop();
//     return res;
//   }

//   bool empty() const {
//     std::lock_guard<std::mutex> lk(mut);
//     return data_queue.empty();
//   }
// };
