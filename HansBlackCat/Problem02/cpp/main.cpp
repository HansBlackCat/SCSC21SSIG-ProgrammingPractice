
#include <iostream>
#include <mutex>
#include <thread>
#include <cmath>
#include "sieve.hpp"

// clang++ main.cpp -std=c++2a -stdlib=libc++

int main()
{
  constexpr int MAX = 100'000'000;

  int sqrtN = static_cast<int>(std::sqrt(MAX));

  std::mutex m;

  std::vector<bool> nVec(MAX);
  nVec[0] = 1;

  std::vector<std::thread> workers;
  workers.reserve(sqrtN);

  for (int i = 1; i < sqrtN; ++i)
  {
    workers.push_back(std::thread(sieving, std::ref(nVec), i + 1, std::ref(m)));
  }

  for (auto &i : workers)
    i.join();

  std::vector<int> prime_numbers;
  prime_numbers.reserve(MAX);

  for (int i = 0; i < nVec.size(); ++i)
  {
    if (!nVec[i])
      prime_numbers.push_back(i + 1);
  }

  std::cout << prime_numbers[100] << '\n';
  std::cout << prime_numbers[1000] << '\n';
  std::cout << prime_numbers[10000] << '\n';
}