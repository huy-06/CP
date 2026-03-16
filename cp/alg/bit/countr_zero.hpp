#include <bit>
#include <type_traits>
#include "../../init/constexpr.hpp"

#ifndef CP_ALG_COUNTR_ZERO
#define CP_ALG_COUNTR_ZERO
namespace cp {
namespace alg {

template <typename Tp>
requires std::is_integral_v<Tp> && (sizeof(Tp) <= 8)
cp_constexpr int countr_zero(Tp x) {
    return std::countr_zero(static_cast<unsigned long long>(x));
}

template <typename Tp>
requires requires(Tp x) { x.val(); }
cp_constexpr int countr_zero(Tp x) {
    unsigned long long low = static_cast<unsigned long long>(x.val());
    if (low != 0) {
        return std::countr_zero(low);
    }
    return 64 + std::countr_zero(static_cast<unsigned long long>(x.val() >> 64));
}

} // namespace alg
} // namespace cp
#endif