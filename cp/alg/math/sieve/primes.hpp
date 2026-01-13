#include <vector>

#ifndef CP_ALG_MATH_SIEVE_PRIMES
#define CP_ALG_MATH_SIEVE_PRIMES
namespace cp {
namespace alg {

// minp[i] == i -> is prime
std::pair<std::vector<int>,
          std::vector<int>> sieve_prime(int n = 1000000) {
    std::vector<int> minp(n + 1, -1), primes;
    primes.reserve(n);

    for (int i = 2; i <= n; ++i) {
        if (minp[i] == -1) {
            minp[i] = i;
            primes.push_back(i);
        }
        for (int p : primes) {
            if (p > minp[i] || i > n / p) break;
            minp[i * p] = p;
        }
    }
    primes.shrink_to_fit();

    return { minp, primes };
}

} // namespace alg
} // namespace cp
#endif