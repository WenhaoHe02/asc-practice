// #include <chrono>
// #include <condition_variable>
// #include <iostream>
// #include <mutex>
// #include <queue>
// #include <thread>
// bool flag = false;
// std::mutex m;
// // 可以用，但是不好控制睡眠时间
// void wait_for_flag() noexcept {
//   std::unique_lock ul(m);
//   while (!flag) {
//     ul.unlock();
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     ul.lock();
//   }
// }

// std::queue<int> q;
// std::mutex m2;
// std::condition_variable cv;
// // 更好的方法:使用condition_variable或condition_variable_any(使用mutex之外的锁)
// void prepare_data(int data) {
//   while (true) {
//     {
//       std::lock_guard lk(m2);
//       q.push(data);
//       std::cout << "push data" << std::endl;
//     }
//     cv.notify_one();
//   }
// }
// void process_data() {
//   while (true) {
//     std::unique_lock lk(m2);
//     cv.wait(lk, [] { return !q.empty(); });
//     q.pop();
//     std::cout << "pop data" << std::endl;
//     lk.unlock();
//   }
// }

// /* wait 的一种实现 */
// /*template<typename Predicate>
// void minimal_wait(std::unique_lock<std::mutex>& lk, Predicate pred) {
//     while (!pred()) {
//         lk.unlock();
//         lk.lock();
//     }
// }
// */
// int main() {
//   std::thread t1(prepare_data, 10);
//   std::thread t2(process_data);
//   t1.join();
//   t2.join();
//   return 0;
// }