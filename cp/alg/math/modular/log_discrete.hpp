#include "pow_mod.hpp"

#ifndef CP_ALG_MOD_LOG
#define CP_ALG_MOD_LOG
namespace cp {
namespace alg {
namespace mod {

template <typename Tp>
Tp log_discrete(Tp a, Tp b, Tp mod) {
    safe_mod(a);
    safe_mod(b);
    if (a == 0) return b == 0 ? 1 : -1;
    Tp k = 1, add = 0, g;
    while ((g = std::gcd(a, mod)) > 1) {
        if (b == k) return add;
        if (b % g) return -1;
        b /= g, mod /= g, ++add;
        k = safe_mul(k, a / g, mod);
    }
    Tp n = std::sqrt(mod) + 1;
    Tp an = pow_mod(a, n, mod);
    std::unordered_map<Tp, Tp> vals;
    for (Tp q = 0, cur = b; q <= n; ++q) {
        vals[cur] = q;
        cur = safe_mul(cur, a, mod);
    }
    for (Tp p = 1, cur = k; p <= n; ++p) {
        cur = safe_mul(cur, an, mod);
        auto it = vals.find(cur);
        if (it !=  vals.end()) {
            Tp ans = n * p - it->second + add;
            return ans;
        }
    }
    return -1;
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif