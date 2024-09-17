#include <iostream>
#include <ranges>
#include <string>
#include <vector>
using namespace std::literals::string_literals;
void user(std::ranges::forward_range auto& fr) {
  std::ranges::filter_view filted_view(fr, [](int x) { return x % 2 != 0; });
  std::ranges::take_view tv{filted_view, 2};
  std::cout << "odd numbers" << std::endl;
  for (auto const& num : tv) {
    std::cout << num << std::endl;
  }
}
// struct Reading {
//   int location{};
//   int temperature{};
//   int humidity{};
//   int air_pressure{};
// };

// int average_temp(std::vector<Reading> readings) {
//   if (readings.size() == 0) {
//     throw No_readings{};
//   }
//   double s = 0;

//   // 通过 views::elements<1> 只查看 temperature（第 1 个成员）
//   for (int x : std::views::elements<1>(readings)) {
//     s += x;
//   }

//   return s / readings.size();
// }

int main() {
  std::vector<int> v{1, 2, 3, 4, 5, 6};
  user(v);
  std::string str = "12"s;
}