#include "safe_mul.hpp"

#ifndef CP_ALG_MOD_POW_MOD
#define CP_ALG_MOD_POW_MOD
namespace cp {
namespace alg {
namespace mod {

template <typename Tp, std::integral Int, typename Mod>
constexpr Mod pow_mod(Tp base, Int exp, Mod mod) {
    Mod res = safe_mod(1, mod);
    Mod b   = safe_mod(base, mod);

    for (; exp > 0; exp >>= 1) {
        if (exp & 1) 
            res = safe_mul(res, b, mod);
        b = safe_mul(b, b, mod);
    }

    return res;
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif