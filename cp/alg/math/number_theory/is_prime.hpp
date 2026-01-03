#include "../modular/pow_mod.hpp"
#include "../../../cst/primes.hpp"

#ifndef CP_ALG_IS_PRIME
#define CP_ALG_IS_PRIME
namespace cp {
namespace alg {

template <typename Tp>
constexpr bool is_prime(const Tp& n) noexcept {
    if (n < 2)
        return false;
    for (int p : cst::small_primes) {
        if (n % p == 0)
            return n == static_cast<Tp>(p);
    }
    Tp d = n - 1;
    int s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        ++s;
    }
    for (int a : cst::test_primes) {
        if (a % n == 0) continue;
        Tp x = mod::pow_mod<Tp>(static_cast<Tp>(a), d, n);
        if (x == 1 || x == n - 1) continue;
        bool composite = true;
        for (int r = 1; r < s; ++r) {
            x = mod::safe_mul(x, x, n);
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