#include <cmath>
#include "../../../ds/numeric/modular/static.hpp"

#ifndef CP_ALG_SQRT
#define CP_ALG_SQRT
namespace cp {
namespace alg {

constexpr long double sqrt(long double n) {
    return std::sqrt(n);
}

constexpr int sqrt(int n) {
    if (n <= 0) {
        return 0;
    }
    int r = std::sqrt(n);
    return r - (r * r > n);
}

constexpr long long sqrt(long long n) {
    if (n <= 0) {
        return 0;
    }
    long long r = std::sqrt(n);
    return r - (r * r > n);
}

#if __cplusplus >= 202002L
template <ds::modular mint>
constexpr mint sqrt(mint n) {
#else
template <unsigned long long mod>
ds::static_mod_int<mod> sqrt(ds::static_mod_int<mod> n) {
    using mint = ds::static_mod_int<mod>;
#endif
    if (n == 0) {
        return 0;
    }
    using value_type = typename mint::value_type;
    value_type p = mint::mod();

    if (n.pow((p - 1) / 2) != 1) {
        return -1;
    }

    if (p % 4 == 3) {
        return n.pow((p + 1) / 4);
    }

    value_type s = 0;
    value_type q = p - 1;
    while ((q & 1) == 0) {
        q >>= 1;
        ++s;
    }

    mint z = 2;
    while (z.pow(p - 1) / 2== 1) {
        ++z;
    }

    mint c = z.pow(q);
    mint r = n.pow((q + 1) / 2);
    mint t = n.pow(q);
    value_type m = s;

    while (t != 1) {
        int i = 1;
        mint tmp = t * t;

        while (tmp != 1) {
            tmp *= tmp;
            ++i;
        }

        mint b = c;
        for (int j = 0; j < m - i - 1; ++j) {
            b *= b;
        }

        m = i;
        c = b * b;
        t *= c;
        r *= b;
    }

    return r;
}

} // namespace alg
} // namespace cp
#endif