#include "safe_mod.hpp"

#ifndef CP_ALG_MOD_SAFE_MUL
#define CP_ALG_MOD_SAFE_MUL
namespace cp {
namespace alg {
namespace mod {

template <typename Tp1, typename Tp2, typename Mod>
constexpr auto safe_mul(Tp1 a, Tp2 b, Mod mod) {
    using uint_t = ds::make_unsigned_t<Mod>;
    uint_t ua = safe_mod(a, mod), ub = safe_mod(b, mod);
    
    if constexpr (sizeof(uint_t) <= 4) {
        return static_cast<uint_t>(1ULL * ua * ub % mod);
    } else {
#ifdef __SIZEOF_INT128__
        return static_cast<uint_t>((static_cast<unsigned __int128>(ua) * ub) % mod);
#else
        uint_t res = 0;
        for (; ub > 0; ub >>= 1) {
            if (ub & 1) res = (res >= mod - ua) ? res - (mod - ua) : res + ua;
            ua = (ua >= mod - ua) ? ua - (mod - ua) : ua + ua;
        }
        return res;
#endif
    }
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif
