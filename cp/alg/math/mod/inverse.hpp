#include <algorithm>
#include "safe_mod.hpp"

#ifndef CP_ALG_MOD_INVERSE
#define CP_ALG_MOD_INVERSE
namespace cp {

namespace alg {
namespace mod {

template <typename Tp>
constexpr std::pair<Tp, Tp> inverse(Tp a, Tp b) {
    using int_t = std::conditional_t<sizeof(Tp) <= 4, long long, __int128_t>;

    int_t r0 = b, r1 = safe_mod(a, b);
    int_t x0 = 0, x1 = 1;

    if (r1 == 0) return { static_cast<Tp>(b), 0 };

    while (r1) {
        int_t q = r0 / r1;
        r0 -= q * r1; std::swap(r0, r1);
        x0 -= q * x1; std::swap(x0, x1);
    }

    if (x0 < 0) x0 += static_cast<int_t>(b) / r0;
    return { static_cast<Tp>(r0), static_cast<Tp>(x0) };
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif