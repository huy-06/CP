#include "../mod/exp.hpp"

#ifndef CP_ALG_PRIME
#define CP_ALG_PRIME
namespace cp {
namespace alg {

template <typename Tp>
constexpr bool is_prime(Tp n) noexcept {
    if (n < 2)
        return false;
    static constexpr int small_primes[12] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (int p : small_primes) {
        if (n % p == 0)
            return n == static_cast<Tp>(p);
    }
    Tp d = n - 1;
    int s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        ++s;
    }
    static constexpr int test_primes[7] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    for (int a : test_primes) {
        if (a % n == 0) continue;
        Tp x = mod::exp<Tp>(static_cast<Tp>(a), d, n);
        if (x == 1 || x == n - 1) continue;
        bool composite = true;
        for (int r = 1; r < s; ++r) {
            x = mod::mul(x, x, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        if (composite)
            return false;
    }
    return true;
}

} // namespace alg
} // namespace cp
#endif