#include <algorithm>
#include "safe_mod.hpp"

#ifndef CP_ALG_MOD_INVERSE
#define CP_ALG_MOD_INVERSE
namespace cp {
namespace alg {
namespace mod {

template <typename Tp>
constexpr std::pair<Tp, Tp> inverse(Tp a, Tp b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0}; 

    long long s = b, t = a;
    long long m0 = 0, m1 = 1;

    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;   
        std::swap(s, t);
        std::swap(m0, m1);
    }

    if (m0 < 0) m0 += b / s;
    return {static_cast<Tp>(s), static_cast<Tp>(m0)};
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif