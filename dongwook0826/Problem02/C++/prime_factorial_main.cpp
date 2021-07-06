#include <iostream>
#include "prime_gen.h"
using std::cout;

int main() {
    cout << "[factorial primes]\n";
    int max_count = 10;
    ull facto = 1*2*3, count = 4;
    cout << "1: 2 = 1! + 1\n"
            << "2: 3 = 2! + 1\n"
            << "3: 5 = 3! - 1\n"
            << "4: 7 = 3! + 1\n";
    // 1! + 1 = 2; 2! + 1 = 3; 3! - 1 = 5; 3! + 1 = 7
    // initial factorial primes
    for (int p = 4; count < max_count; p++) {
        facto *= p;
        if (IsPrime(facto - 1))
            cout << ++count << ": " << facto - 1 << " = "
                    << p << "! - 1\n";
        if (count >= max_count) break;
        // Wilson's theorem; (n-1)! === -1 (mod n) iff n is prime
        if (!IsPrime(p + 1) && IsPrime(facto + 1))
            cout << ++count << ": " << facto + 1 << " = "
                    << p << "! + 1\n";
    }
    /*
    5: 23 = 4! - 1
    6: 719 = 6! - 1
    7: 5039 = 7! - 1
    8: 39916801 = 11! + 1
    9: 479001599 = 12! - 1
    10: 87178291199 = 14! - 1
    */
    return 0;
}
