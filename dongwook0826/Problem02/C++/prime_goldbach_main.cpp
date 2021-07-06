#include <iostream>
#include "prime_gen.h"
using std::cout;

int main() {
    cout << "[Wrong Goldbach!]\n";
    int max_count = 2;
    VecUll primes{2};
    VecUll non_goldbach_comp{};
    for (ull num = 3; non_goldbach_comp.size() < max_count; num += 2) {
        bool is_prime = true;
        for (int i = 0; i < primes.size() && primes[i] <= num / primes[i]; i++) {
            if (num % primes[i] == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            primes.push_back(num);
            continue;
        }
        bool is_goldbach = false;
        for (ull k = 2, n = num; n > k + 2; k += 4) {
            n -= k;
            if (BinarySearch(primes, n, false) >= 0) {
                is_goldbach = true;
                break;
            }
        }
        if (!is_goldbach)
            non_goldbach_comp.push_back(num);
    }
    for (ull cmp : non_goldbach_comp)
        cout << cmp << '\n';
    // This sequence is probably finite,
    // where the only known exceptions are 5777 and 5993.
    return 0;
}