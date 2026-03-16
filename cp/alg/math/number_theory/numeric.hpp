#include <algorithm>
#include "../../bit/countr_zero.hpp"
#include "../../../ds/misc/make_unsigned.hpp"

#ifndef CP_ALG_NUMERIC
#define CP_ALG_NUMERIC
namespace cp {
namespace alg {

template <typename Tp>
cp_constexpr Tp gcd(Tp a, Tp b) {
    if (!a) return b < 0 ? -b : b;
    if (!b) return a < 0 ? -a : a;

    using unsigned_type = typename ds::make_unsigned<Tp>::type;
    unsigned_type ua = a < 0 ? 0 - static_cast<unsigned_type>(a) : static_cast<unsigned_type>(a);
    unsigned_type ub = b < 0 ? 0 - static_cast<unsigned_type>(b) : static_cast<unsigned_type>(b);
    
    int shift = alg::countr_zero(ua | ub);
    ua >>= alg::countr_zero(ua);

    do {
        ub >>= alg::countr_zero(ub);
        if (ua > ub) {
            std::swap(ua, ub);
        }
        ub -= ua;
    } while (ub != 0);

    return static_cast<Tp>(ua << shift);
}

template <typename Tp>
cp_constexpr Tp lcm(Tp a, Tp b) {
    if (!a || !b) return 0;
    Tp res = a / gcd(a, b) * b;
    return res < 0 ? -res : res;
}


} // namespace alg
} // namespace cp
#endif