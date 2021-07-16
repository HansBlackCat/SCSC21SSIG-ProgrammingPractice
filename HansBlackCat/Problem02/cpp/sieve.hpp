#ifndef __SIEVE_HPP
#define __SIEVE_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <cassert>

void sieving(std::vector<bool> &vec, const int n, std::mutex &m)
{
  if (vec[n - 1] == 1)
    return;

  for (int i = n * n; i <= vec.size(); i += n)
  {
    m.lock();
    vec[i - 1] = 1;
    m.unlock();
  }
};

#endif //__SIEVE_HPP