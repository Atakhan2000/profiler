#include "../test/snippets.h"
#include <cmath>

uint_least64_t fp::factorial(uint_least64_t n)
{
  return (n <= 1) ? 1 : (n * factorial(n - 1));
}

double fp::foo(int m) {
  double sum = 0;
  for (int i = 0; i < 1000000; i++) {
    sum += std::cos(42. * m * i * i);
  }
  for (int i = 0; i < 100000; i++) {
  }
  return std::abs(sum);
}
