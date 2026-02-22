#include "safe_mod.hpp"

#ifndef CP_ALG_MOD_SAFE_MUL
#define CP_ALG_MOD_SAFE_MUL
namespace cp {

namespace internal {

constexpr unsigned long long safe_mul(unsigned long long a, unsigned long long b, unsigned long long mod, unsigned long long res) {
    return b == 0 ? res : safe_mul(
        a >= mod - a ? a + a - mod : a + a,
        b >> 1,
        mod, 
        (b & 1) ? (res >= mod - a ? res + a - mod : res + a) : res
    );
}

} // namespace internal

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
    return internal::safe_mul(a, b, mod, 0);
#endif
}

constexpr unsigned long long safe_mul(long long a, long long b, unsigned long long mod) {
    return safe_mul(static_cast<unsigned long long>(safe_mod(a, mod)), static_cast<unsigned long long>(safe_mod(b, mod)), mod);
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif
