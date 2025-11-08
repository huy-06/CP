#include <vector>

#ifndef HUY_ALG_MATH_SIEVE_PRIME
#define HUY_ALG_MATH_SIEVE_PRIME
namespace huy {
namespace alg {

std::pair<std::vector<int>,
          std::vector<int>> sieve_prime(int n = 1000000) {
    std::vector<int> minp(n + 1, -1), primes;
    primes.reserve(std::max(1, n / 10));

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

    return { minp, primes };
}

} // namespace alg
} // namespace huy
#endif