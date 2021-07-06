#include <iostream>
#include "prime_gen.h"

VecUll PrimeGen::prime_list_{2ull, 3ull};

void PrimeGen::BoundGen(ull bnd) {
    int size = prime_list_.size();
    ull num = prime_list_[size - 1];
    if (bnd <= num)
        return;
    num += 2;
    bool isPrime;
    while (num <= bnd) {
        isPrime = true;
        for (int i = 0; i < size && prime_list_[i] <= num / prime_list_[i]; i++) {
            if (num % prime_list_[i] == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime)
            prime_list_.push_back(num);
        num += 2;
    }
}

void PrimeGen::CountGen(int cnt) {
    if (cnt < 0)
        std::cout << "!!! invalid args: cnt negative !!!\n";
    int size = prime_list_.size();
    if (cnt < size)
        return;
    prime_list_.resize(cnt);
    ull num = prime_list_[size - 1] + 2ull;
    bool isPrime;
    while (size < cnt) {
        isPrime = true;
        for (int i = 0; i < size && prime_list_[i] <= num / prime_list_[i]; i++) {
            if (num % prime_list_[i] == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime)
            prime_list_[size++] = num;
        num += 2;
    }
}

int BinarySearch(const VecUll &vec, const ull target, const bool forceInd = false) {
    // vec must be sorted in ascending order
    int lo = 0, hi = vec.size() - 1;
    int md;
    while (lo <= hi) {
        md = (lo + hi) / 2;
        if (vec[md] == target) return md;
        if (vec[md] < target) lo = md + 1;
        else                  hi = md - 1;
    } return forceInd ? lo : -1;
}

ull sqrt(ull num) {
    ull asq = num / 2;
    do
        asq = (asq + num / asq) / 2;
    while (asq > num / asq);
    return asq;
}

bool IsPrime(ull num) {
    VecUll& primes = PrimeGen::prime_list();
    ull max_prime = primes[primes.size() - 1];
    if (num <= max_prime)
        return BinarySearch(primes, num) >= 0;
    if (max_prime <= num / max_prime)
        PrimeGen::BoundGen(sqrt(num));
    for (int i = 0; i < primes.size() && primes[i] <= num / primes[i]; i++) {
        if (num % primes[i] == 0)
            return false;
    } return true;
}

ull NthPrime(int n) {
    PrimeGen::CountGen(n);
    return PrimeGen::prime_list()[n - 1];
}
