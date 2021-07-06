#include <iostream>
#include "prime_gen.h"
using std::cout;

ull gcd(ull a, ull b) {
    if (a == 0) return b;
    if (b == 0) return a;
    if (a < b) std::swap(a, b);
    ull r;
    while (b) {
        r = a % b;
        a = b;
        b = r;
    } return a;
} // if return value == 1, they are co-prime

ull pow(ull base, ull exp) {
    if (base == 0)
        return exp ? 0 : 1;
    if (base == 1)
        return 1;
    ull bit = 1;
    while (bit <= exp) bit <<= 1;
    ull prod = 1;
    for (bit >>= 1; bit; bit >>= 1) {
        prod *= prod;
        if (bit & exp)
            prod *= base;
    }
    return prod;
}

int main() {
    cout << "[Leyland primes]\n";
    int max_count = 5;
    VecUll exps{0, 0, 3};
    VecUll next_num{0, 0, 0};
    VecUll leyland_primes{}, temp_primes;
    if (leyland_primes.size() == 0)
    while (leyland_primes.size() < max_count) {
        temp_primes = {};
        next_num[2] = pow(2, exps[2]) + pow(exps[2], 2);
        if (IsPrime(next_num[2]))
            temp_primes.push_back(next_num[2]);
        for (int n = 3; ; n++) {
            if (exps.size() <= n) {
                exps.push_back(n + 1);
                next_num.push_back(pow(n, exps[n]) + pow(exps[n], n));
            }
            if (next_num[n] > next_num[2])
                break;
            if (IsPrime(next_num[n]))
                temp_primes.insert(
                    temp_primes.begin() + BinarySearch(temp_primes, next_num[n], true),
                    next_num[n]
                );
            do {
                do 
                    exps[n] += 2;
                while (gcd(exps[n], n) > 1);
                next_num[n] = pow(n, exps[n]) + pow(exps[n], n);
                if (next_num[n] > next_num[2])
                    break;
                if (IsPrime(next_num[n]))
                    temp_primes.insert(
                        temp_primes.begin() + BinarySearch(temp_primes, next_num[n], true),
                        next_num[n]
                    );
            } while (1);
        }
        exps[2] += 2;
        leyland_primes.insert(leyland_primes.end(), temp_primes.begin(), temp_primes.end());
    }
    for (ull lp : leyland_primes)
        cout << lp << '\n';
    return 0;
}
