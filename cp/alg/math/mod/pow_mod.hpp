#include "safe_mul.hpp"

#ifndef CP_ALG_MOD_POW_MOD
#define CP_ALG_MOD_POW_MOD
namespace cp {
namespace alg {
namespace mod {

template<typename Tp, std::integral Int>
constexpr Tp pow_mod(Tp base, Int exp, Tp mod) {
    Tp res = 1 % mod;
    base = safe_mod(base, mod);
    for (; exp > 0; exp >>= 1) {
        if (exp & 1) res = static_cast<Tp>(safe_mul(res, base, mod));
        base = static_cast<Tp>(safe_mul(base, base, mod));
    }
    return res;
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif