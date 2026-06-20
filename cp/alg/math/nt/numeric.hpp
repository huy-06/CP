#include <algorithm>
#include "../../../init/constexpr.hpp"

#ifndef CP_ALG_NUMERIC
#define CP_ALG_NUMERIC
namespace cp {
namespace alg {

template <typename Tp>
cp_constexpr Tp gcd(Tp a, Tp b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;

    while (b != 0) {
        a %= b;
        std::swap(a, b);
    }

    return a;
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