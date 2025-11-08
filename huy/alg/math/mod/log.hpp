#include "exp.hpp"

#ifndef HUY_ALG_MOD_LOG
#define HUY_ALG_MOD_LOG
namespace huy {
namespace alg {
namespace mod {

template <typename Tp>
Tp log(Tp a, Tp b, Tp mod) {
    safe(a);
    safe(b);
    if (a == 0) return b == 0 ? 1 : -1;
    Tp k = 1, add = 0, g;
    while ((g = std::gcd(a, mod)) > 1) {
        if (b == k) return add;
        if (b % g) return -1;
        b /= g, mod /= g, ++add;
        k = mul(k, a / g, mod);
    }
    Tp n = std::sqrt(mod) + 1;
    Tp an = exp(a, n, mod);
    std::unordered_map<Tp, Tp> vals;
    for (Tp q = 0, cur = b; q <= n; ++q) {
        vals[cur] = q;
        cur = mul(cur, a, mod);
    }
    for (Tp p = 1, cur = k; p <= n; ++p) {
        cur = mul(cur, an, mod);
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
} // namespace huy
#endif