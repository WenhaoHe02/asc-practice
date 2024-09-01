// #include <future>
// #include <list>
// // 用函数式编程实现并发快速排序
// template <typename T>
// std::list<T> parallel_quick_sort(std::list<T> input) {
//   if (input.empty()) {
//     return input;
//   }
//   std::list<T> result;
//   result.splice(result.begin(), input, input.begin());
//   T const& pivot    = *result.begin();
//   auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });
//   std::list<T> lower_part;
//   lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
//   std::future<std::list<T>> new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));  // 并行排序下部分
//   auto new_higher = parallel_quick_sort(std::move(input));  // 递归排序上部分
//   result.splice(result.end(), new_higher);                  // 拼接排序后的上部分
//   result.splice(result.begin(), new_lower.get());           // 获取并拼接排序后的下部分
//   return result;
// }
