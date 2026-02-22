#include "safe_mul.hpp"

#ifndef CP_ALG_MOD_POW_MOD
#define CP_ALG_MOD_POW_MOD
namespace cp {

namespace internal {

template <typename Tp, typename Int>
constexpr Tp pow_mod(Tp base, Int exp, Tp mod, Tp res) {
    return exp == 0 ? res :
            pow_mod<Tp, Int>(
                static_cast<Tp>(alg::mod::safe_mul(base, base, mod)),
                exp >> 1, 
                mod, 
                static_cast<Tp>((exp & 1) ? alg::mod::safe_mul(res, base, mod) : res)
            );
}

} // namespace internal

namespace alg {
namespace mod {

template<typename Tp, typename Int>
constexpr Tp pow_mod(Tp base, Int exp, Tp mod) {
    return internal::pow_mod(safe_mod(base, mod), exp, mod, static_cast<Tp>(1));
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif