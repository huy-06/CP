#include <algorithm>
#include "safe_mod.hpp"

#ifndef CP_ALG_MOD_INVERSE
#define CP_ALG_MOD_INVERSE
namespace cp {

namespace internal {

template <typename Tp>
constexpr std::pair<Tp, Tp> inverse(long long s, long long t, long long m0, long long m1, Tp b) {
    return t == 0 ? 
        std::pair<Tp, Tp>{static_cast<Tp>(s), static_cast<Tp>(m0 < 0 ? m0 + static_cast<long long>(b / s) : m0)} :
        inverse<Tp>(t, s - t * (s / t), m1, m0 - m1 * (s / t), b);
}

} // namespace internal

namespace alg {
namespace mod {

template <typename Tp>
constexpr std::pair<Tp, Tp> inverse(Tp a, Tp b) {
    return safe_mod(a, b) == 0 ? std::pair<Tp, Tp>{b, 0} :
           internal::inverse<Tp>(b, safe_mod(a, b), 0, 1, b);
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif