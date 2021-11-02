#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <thread>
#include "../profiler.h"
#include "../test/snippets.h"

//constexpr size_t N = 10000;

uint_least64_t function(uint_least64_t n)
{
  PROFILE_FUNCTION();

  // This shows the difference between the thread and the steady clocks
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  return fp::factorial(n);
}

uint_least64_t call_function(size_t n)
{
  uint_least64_t result{0};

  for (size_t i = 0; i < n; ++i) {
    result = function(20);
  }

  return result;
}

double function2(int m)
{
  PROFILE_FUNCTION();

  return fp::foo(m);
}


double call_function2(size_t n)
{
  double result{0};

  for (size_t i = 0; i < n; ++i) {
    result = function2(10);
  }

  return result;
}

int main(int, char **)
{
  size_t N = 10000;
  const auto result = std::async(std::launch::async, call_function, N).get();
  size_t k = 100;
  const auto result2 = call_function2(k);

  std::cout << "Factorial(20) => " << result << " == " << 2432902008176640000ull << " <= Should be" << std::endl;
  std::cout << "Number of calls should be #" << N << std::endl;
  std::cout << "Result of fun2 == " << result2 << std::endl;
  std::cout << "Number of calls should be #" << k << std::endl;

  return 0;
}


