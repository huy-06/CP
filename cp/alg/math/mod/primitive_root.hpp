#include "pow_mod.hpp"

#ifndef CP_ALG_MATH_MOD_PRIME_ROOT
#define CP_ALG_MATH_MOD_PRIME_ROOT
namespace cp {
namespace alg {
namespace mod {

template <typename Tp>
constexpr Tp primitive_root(Tp m) {
    if (m == 2) return 1;
    if (m == 167772161 || m == 469762049 || m == 998244353) return 3;
    if (m == 754974721) return 11;

    Tp divs[40]{}, x = m - 1;
    int cnt = 0;
    
    for (Tp i = 2; i * i <= x; ++i) {
        if (x % i == 0) {
            divs[cnt++] = i;
            while (x % i == 0) x /= i;
        }
    }
    if (x > 1) divs[cnt++] = x;

    for (Tp g = 2; ; ++g) {
        bool ok = true;
        for (int i = 0; i < cnt; ++i) {
            if (pow_mod(g, (m - 1) / divs[i], m) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif
