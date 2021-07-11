#include <iostream>
#include "prime_gen.h"
using std::cout;

int main() {
    cout << "[prime counting]\n"
         << NthPrime(101) << '\n'
         << NthPrime(1001) << '\n'
         << NthPrime(10001) << '\n'
         << NthPrime(100001) << '\n';
    // each 547, 7927, 104743, 1299721
    return 0;
}
