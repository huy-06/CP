#include <algorithm>
#include "safe.hpp"

#ifndef HUY_ALG_MOD_INV_GCD
#define HUY_ALG_MOD_INV_GCD
namespace huy {
namespace alg {
namespace mod {

template <typename Tp>
constexpr std::pair<Tp, Tp> inv_gcd(Tp a, Tp b) {
    a = safe(a, b);
    if (a == 0) return {b, 0}; 
    Tp s = b, t = a;
    Tp m0 = 0, m1 = 1;
    while (t) {
        Tp u = s / t;
        s -= t * u;
        m0 -= m1 * u;   
        std::swap(s, t);
        std::swap(m0, m1);
    }
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

} // namespace mod
} // namespace alg
} // namespace huy
#endif