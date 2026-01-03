#include "safe_mul.hpp"

#ifndef CP_ALG_MOD_POW_MOD
#define CP_ALG_MOD_POW_MOD
namespace cp {
namespace alg {
namespace mod {

template<typename Tp, typename Int>
constexpr Tp pow_mod(Tp base, Int exp, Tp mod) {
    Tp res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) {
           res = safe_mul(res, base, mod); 
        }
        base = safe_mul(base, base, mod);
        exp >>= 1;
    }
    return res;
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif