#include <bitset>
#include <iostream>
#include <ostream>
#include <span>
#include <utility>
void forLoop(std::span<int> s) {
  for (int& ele : s) {
    std::cout << ele << " ";
  }
}
void binary(int num) {
  std::bitset<8 * sizeof(int)> b = num;
  std::bitset<9> b2{"111111111"};
  std::cout << b << std::endl;
  std::cout<<b[1]<<std::endl;
}
template<typename  Entry, typename ErrorCode>
std::pair<Entry*, ErrorCode> complex_search() {
  Entry* e = nullptr;
  ErrorCode ec = 1;
  return {e, ec};
}
void user() {
  auto [e, ec] = complex_search<int, int>();
  if (ec == 1) {
    //异常处理
  }
}
void f(std::vector<std::string>& v) {
  std::pair p1{v.begin(), 2};              // 一种方式
  auto p2 = make_pair(v.begin(), 2);  // 另一种方式
                                      // ...
}
void binary2(int num) {
  std::bitset<8 * sizeof(int)> b = num;
  std::cout << b.to_string() << std::endl;
}

template <size_t N = 0, typename... Ts>
constexpr void print(std::tuple<Ts...> tup) {
  if constexpr (N < sizeof...(Ts)) {  // 检查 N 是否小于 tuple 的元素个数
    std::cout << get<N>(tup) << ' ';       // 打印第 N 个元素
    print<N + 1>(tup);                // 递归调用，处理下一个元素
  }
}
// variant<string, Error_code> compose_message(istream& s) {
//   string mess;
//   // ... 从 s 读取并组合消息 ...
//   if (no_problems) {
//     return mess;  // 返回一个字符串
//   } else {
//     return Error_code{some_problem};  // 返回一个 Error_code
//   }
// }


// using Node = variant<Expression, Statement, Declaration, Type>;

// void check(Node* p) {
//   if (holds_alternative<Expression>(*p)) {
//     Expression& e = get<Expression>(*p);
//     // ...
//   } else if (holds_alternative<Statement>(*p)) {
//     Statement& s = get<Statement>(*p);
//     // ...
//   }
//   // ... Declaration 和 Type ...
// }

// optional<string> compose_message(istream& s) {
//   string mess;
//   // ... 从 s 中读取并组合消息 ...
//   if (no_problems) {
//     return mess;  // 返回消息
//   }
//   return {};  // 空的 optional
// }

// any compose_message(istream& s) {
//   string mess;
//   // ... 从 s 中读取并组合消息 ...
//   if (no_problems) {
//     return mess;  // 返回一个字符串
//   } else {
//     return error_number;  // 返回一个整数
//   }
// }

//  