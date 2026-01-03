#include <type_traits>
#include "../../../init/constexpr.hpp"

#ifndef CP_ALG_MOD_SAFE_MOD
#define CP_ALG_MOD_SAFE_MOD
namespace cp {
namespace alg {
namespace mod {

template <typename Tp1, typename Tp2>
constexpr Tp2 safe_mod(Tp1 a, Tp2 mod) {
#if __cplusplus >= 202002L
    if constexpr (std::is_signed_v<Tp1>)
#endif
    if (a < static_cast<Tp1>(0)) {
        unsigned long long u_mod = static_cast<unsigned long long>(mod);

        unsigned long long res = (static_cast<unsigned long long>(-(a + 1)) + 1) % u_mod;

        if (res == 0) {
            return static_cast<Tp2>(0);
        }

        return static_cast<Tp2>(u_mod - res);
    }

    return static_cast<Tp2>(static_cast<unsigned long long>(a) % static_cast<unsigned long long>(mod));
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif