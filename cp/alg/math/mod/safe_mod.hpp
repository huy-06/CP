// #include "../../../init/optimize.hpp"
#include "../../../ds/misc/make_unsigned.hpp"

#ifndef CP_ALG_MOD_SAFE_MOD
#define CP_ALG_MOD_SAFE_MOD
namespace cp {
namespace alg {
namespace mod {

template <typename Tp1, typename Tp2>
constexpr Tp2 safe_mod(Tp1 a, Tp2 mod) {
    using max_t = std::conditional_t<(sizeof(Tp1) > sizeof(Tp2)), Tp1, Tp2>; 
    using uint_t = std::conditional_t<sizeof(max_t) <= 4, unsigned int, ds::make_unsigned_t<max_t>>;
    
    uint_t umod = static_cast<uint_t>(mod);
    if constexpr (std::is_signed_v<Tp1>) {
        if (a >= 0 && static_cast<uint_t>(a) < umod) return static_cast<Tp2>(a);
        if (a < 0) {
            uint_t r = (static_cast<uint_t>(0) - static_cast<uint_t>(a)) % umod;
            return static_cast<Tp2>(r == 0 ? 0 : umod - r);
        }
    } else {
        if (static_cast<uint_t>(a) < umod) return static_cast<Tp2>(a);
    }
    
    return static_cast<Tp2>(static_cast<uint_t>(a) % umod);
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif