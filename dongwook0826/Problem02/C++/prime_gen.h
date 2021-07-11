#ifndef TENDONG_PRIME_GEN
#define TENDONG_PRIME_GEN

#include <vector>
typedef unsigned long long ull;
typedef std::vector<ull> VecUll;

typedef class _tendong_prime_gen_algorithm {
    public:
        static void BoundGen(ull);
        static void CountGen(int);
        static VecUll &prime_list() { return prime_list_; }
    private:
        static VecUll prime_list_; // prime container
} PrimeGen;

int BinarySearch(const VecUll&, const ull, const bool);
bool IsPrime(ull);
ull NthPrime(int);

#endif
