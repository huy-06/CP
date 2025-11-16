#include "prime.hpp"

#ifndef CP_ALG_MATH_SIEVE_PHI
#define CP_ALG_MATH_SIEVE_PHI
namespace cp {
namespace alg {

std::vector<int> sieve_phi(int n = 1000000) {
    const std::vector<int>& minp = sieve_prime(n).first;
    std::vector<int> phi(n + 1, 0);

    if (n >= 1) phi[1] = 1;
    for (int i = 2; i <= n; ++i) {
        int p = minp[i];
        int m = i / p;
        if (m % p == 0) {
            phi[i] = phi[m] * p;
        } else {
            phi[i] = phi[m] * (p - 1);
        }
    }
    
    return phi;
}

} // namespace alg
} // namespace cp
#endif