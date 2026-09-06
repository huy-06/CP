#include "../mod/pow_mod.hpp"
// #include "../../../cst/primes.hpp"

#ifndef CP_ALG_IS_PRIME
#define CP_ALG_IS_PRIME
namespace cp {
namespace alg {

template <typename Tp>
constexpr bool is_prime(Tp n) {
    if constexpr (std::is_signed_v<Tp>) {
        if (n < 2) return false;
    }
    
    using uint_t = ds::make_unsigned_t<Tp>;
    uint_t un = static_cast<uint_t>(n);
    
    if (un < 2) 
        return false;
    if (un == 2 || un == 3) 
        return true;
    if (un % 2 == 0) 
        return false;

    constexpr uint_t small_primes[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (uint_t p : small_primes) {
        if (un == p) 
            return true;
        if (un % p == 0) 
            return false;
    }

    uint_t d = un - 1;
    int s = std::countr_zero(d);
    d >>= s;

    auto test_miller_rabin = [&](uint_t a) constexpr -> bool {
        if (a >= un) return true; 
        uint_t x = mod::pow_mod(a, d, un);
        if (x == 1 || x == un - 1) return true;
        for (int r = 1; r < s; ++r) {
            x = mod::safe_mul(x, x, un);
            if (x == un - 1) return true;
        }
        return false;
    };

    if constexpr (sizeof(uint_t) <= 4) {
        constexpr uint_t bases[] = {2, 7, 61};
        for (uint_t a : bases) {
            if (!test_miller_rabin(a)) 
                return false;
        }
    } else {
        constexpr uint_t bases[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
        for (uint_t a : bases) {
            if (!test_miller_rabin(a)) 
                return false;
        }
    }
    
    return true;
}

} // namespace alg
} // namespace cp
#endif