#include <iostream>

int func() {
  return 228;
}

int main() {
  const int i = 228;

  auto f = []{ std::cout << i << '\n';  };

  f();

  return 0;
}
