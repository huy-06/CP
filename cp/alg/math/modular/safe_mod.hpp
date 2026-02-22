#include <type_traits>
#include "../../../init/optimize.hpp"

#ifndef CP_ALG_MOD_SAFE_MOD
#define CP_ALG_MOD_SAFE_MOD
namespace cp {

namespace internal {

template <bool IsSigned>
struct safe_mod;

template <>
struct safe_mod<true> {
    template <typename Tp1, typename Tp2>
    static constexpr Tp2 calc(Tp1 a, Tp2 mod) {
        return (a >= 0 && static_cast<unsigned long long>(a) < static_cast<unsigned long long>(mod))
            ? static_cast<Tp2>(a)
            : (
                a < static_cast<Tp1>(0) ? 
                static_cast<Tp2>(
                    ((static_cast<unsigned long long>(-(a + 1)) + 1) % static_cast<unsigned long long>(mod) == 0) ? 0 : 
                    (static_cast<unsigned long long>(mod) - ((static_cast<unsigned long long>(-(a + 1)) + 1) % static_cast<unsigned long long>(mod)))
                ) :
                static_cast<Tp2>(static_cast<unsigned long long>(a) % static_cast<unsigned long long>(mod))
            );
    }
};

template <>
struct safe_mod<false> {
    template <typename Tp1, typename Tp2>
    static constexpr Tp2 calc(Tp1 a, Tp2 mod) {
        return (static_cast<unsigned long long>(a) < static_cast<unsigned long long>(mod))
            ? static_cast<Tp2>(a)
            : static_cast<Tp2>(static_cast<unsigned long long>(a) % static_cast<unsigned long long>(mod));
    }
};

} // namesapce internal

namespace alg {
namespace mod {

template <typename Tp1, typename Tp2>
constexpr Tp2 safe_mod(Tp1 a, Tp2 mod) {
    return internal::safe_mod<std::is_signed<Tp1>::value>::calc(a, mod);
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif