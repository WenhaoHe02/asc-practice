#include <iterator>
#include <ranges>
// import my_module
template <typename Iter>
concept my_forward_iterator = requires(Iter p, int i) {
                                p[i];
                                p + i;
                              };
template <my_forward_iterator Iter>
void advance(Iter p, int n) {
  p += n;
}

template <std::forward_iterator Iter>
  requires requires(Iter p, int i) {
             p[i];
             p + i;
           }
void advance(Iter p, int n) {
  p += n;
}

template <typename B>
concept Boolean = std::is_convertible_v<B, bool>;

template <typename T, typename T2 = T>
concept Equality_comparable = requires(T a, T2 b) {
                                { a == b } -> Boolean;  // 使用 == 比较 T 和 T2
                                { a != b } -> Boolean;  // 使用 != 比较 T 和 T2
                                { b == a } -> Boolean;  // 使用 == 比较 T2 和 T
                                { b != a } -> Boolean;  // 使用 != 比较 T2 和 T
                              };

template <typename S>
concept Sequence = requires(S a) {
                     typename std::ranges::range_value_t<S>;                     // 使用 std::ranges 的类型
                     typename std::ranges::iterator_t<S>;                        // 使用 std::ranges 的类型
                     { a.begin() } -> std::same_as<std::ranges::iterator_t<S>>;  // 检查 begin() 的返回类型
                     { a.end() } -> std::same_as<std::ranges::iterator_t<S>>;    // 检查 end() 的返回类型
                     requires std::input_iterator<std::ranges::iterator_t<S>>;   // 检查是否是 input_iterator
                     requires std::same_as<std::ranges::range_value_t<S>,
                                           std::iter_value_t<std::ranges::iterator_t<S>>>;  // 检查值类型
                   };
