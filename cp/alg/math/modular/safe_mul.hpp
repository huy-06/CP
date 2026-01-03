#include "safe_mod.hpp"

#ifndef CP_ALG_MOD_SAFE_MUL
#define CP_ALG_MOD_SAFE_MUL
namespace cp {
namespace alg {
namespace mod {

constexpr unsigned safe_mul(unsigned a, unsigned b, unsigned mod) {
    return static_cast<unsigned long long>(a) * b % mod;
}

constexpr unsigned safe_mul(int a, int b, unsigned mod) {
    return static_cast<unsigned long long>(safe_mod(a, mod)) * safe_mod(b, mod) % mod;
}

constexpr unsigned long long safe_mul(unsigned long long a, unsigned long long b, unsigned long long mod) {
#ifdef __SIZEOF_INT128__
    return static_cast<unsigned long long>(static_cast<__uint128_t>(a) * b % mod);
#else
    unsigned long long res = 0;
    while (b > 0) {
        if (b & 1) {
            res += a;
            if (res >= mod) res -= mod;
        }
        a <<= 1;
        if (a >= mod) a -= mod;
        b >>= 1;
    }
    return res;
#endif
}

constexpr unsigned long long safe_mul(long long a, long long b, unsigned long long mod) {
    return safe_mul(static_cast<unsigned long long>(safe_mod(a, mod)), static_cast<unsigned long long>(safe_mod(b, mod)), mod);
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif
