#ifndef __SUB_PRIME_HPP
#define __SUB_PRIME_HPP

#include <algorithm>
#include <cstdio>
#include <vector>
#include <memory>

class factorial
{
private:
  int i, n, m;
  bool minus;

  void fac_next()
  {
    i = ++i;
    n *= i;
  }

public:
  explicit factorial() : i(1), n(1), minus(false){};
  factorial(factorial &&) = delete;
  factorial(const factorial &) = delete;
  factorial &operator=(const factorial &) = delete;
  factorial &operator=(factorial &&) = delete;

  int next()
  {
    if ((n > 2) && minus)
    {
      minus = false;
      return n - 1;
    }
    else
    {
      minus = true;
      m = n + 1;
      fac_next();
      return m;
    }
  }
};

unsigned long long int_pow(int a, int b)
{
  unsigned long long ans = a;
  int cnt = b;
  while (--cnt)
  {
    ans *= a;
  }
  return ans;
}

bool is_prime(int i, std::vector<bool> &sieve)
{
  return !sieve[i - 1];
};

int factorial_prime(int number, std::vector<bool> &sieve)
{
  auto fac = std::make_unique<factorial>();
  int cur;

  while (number)
  {
    cur = fac->next();
    if (!sieve[cur - 1])
      --number;
  }

  return cur;
}

unsigned long long leyland_prime(int number, std::vector<bool> &sieve)
{
  int n = 3;
  int cnt = number;
  std::vector<unsigned long long> uVec;
  int max = sieve.size();
  unsigned long long tmp;

  while (true)
  {
    for (int i = 2; i < n; ++i)
    {
      tmp = int_pow(i, n) + int_pow(n, i);
      if (tmp > max)
        continue;
      if (is_prime(tmp, std::ref(sieve)))
      {
        uVec.push_back(tmp);
        std::sort(uVec.begin(), uVec.end());
      }

      if (uVec.size() >= cnt)
      {
        if (i == 2 || *(uVec.end() - 1) < tmp)
        {
          return uVec[number - 1];
        }
      }
    }

    ++n;
  }
}

#endif