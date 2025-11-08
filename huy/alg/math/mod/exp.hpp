#include "mul.hpp"

#ifndef HUY_ALG_MOD_EXP
#define HUY_ALG_MOD_EXP
namespace huy {
namespace alg {
namespace mod {

template<typename Tp, typename Int>
constexpr Tp exp(Tp base, Int exp, Tp mod) {
    Tp res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) 
            res = mul(res, base, mod);
        base = mul(base, base, mod);
        exp >>= 1;
    }
    return res;
}

} // namespace mod
} // namespace alg
} // namespace huy
#endif