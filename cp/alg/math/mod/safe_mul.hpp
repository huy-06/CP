#include "safe_mod.hpp"

#ifndef CP_ALG_MOD_SAFE_MUL
#define CP_ALG_MOD_SAFE_MUL
namespace cp {
namespace alg {
namespace mod {

template <typename Tp1, typename Tp2, typename Mod>
constexpr Mod safe_mul(Tp1 a, Tp2 b, Mod mod) {
    using uint_t = ds::make_unsigned_t<Mod>;
    uint_t ua = safe_mod(a, mod);
    uint_t ub = safe_mod(b, mod);
    uint_t umod = static_cast<uint_t>(mod);

    if constexpr (sizeof(uint_t) <= 4) {
        return static_cast<Mod>(1ULL * ua * ub % umod);
    } else {
#ifdef __SIZEOF_INT128__
        return static_cast<Mod>((static_cast<unsigned __int128>(ua) * ub) % umod);
#else
        uint_t res = 0;
        for (; ub > 0; ub >>= 1) {
            if (ub & 1) res = (res >= umod - ua) ? res - (umod - ua) : res + ua;
            ua = (ua >= umod - ua) ? ua - (umod - ua) : ua + ua;
        }
        return static_cast<Mod>(res);
#endif
    }
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif
