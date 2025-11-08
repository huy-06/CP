#include "prime.hpp"

#ifndef HUY_ALG_MATH_SIEVE_MOBIUS
#define HUY_ALG_MATH_SIEVE_MOBIUS
namespace huy {
namespace alg {

std::vector<int> sieve_mobius(int n = 1000000) {
    const std::vector<int>& minp = sieve_prime(n).first;
    std::vector<int> mu(n + 1, 0);

    if (n >= 1) mu[1] = 1;
    for (int i = 2; i <= n; ++i) {
        int p = minp[i];
        int m = i / p;
        if (m % p == 0) {
            mu[i] = 0;
        } else {
            mu[i] = -mu[m];
        }
    }

    return mu;
}

} // namespace alg
} // namespace huy
#endif