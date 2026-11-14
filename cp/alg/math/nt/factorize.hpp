#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include "is_prime.hpp"

#ifndef CP_ALG_MATH_NUM_PRIME
#define CP_ALG_MATH_NUM_PRIME
namespace cp {

namespace internal {

template <typename Tp>
constexpr Tp prho_f(Tp x, Tp c, Tp n) {
    using uint_t = ds::make_unsigned_t<Tp>;
    uint_t un = static_cast<uint_t>(n);
    uint_t ux = static_cast<uint_t>(x);
    uint_t uc = static_cast<uint_t>(c);
    uint_t sq = alg::math::safe_mul(ux, ux, un);
    uint_t res = (sq >= un - uc) ? (sq - (un - uc)) : (sq + uc);
    return static_cast<Tp>(res);
}

constexpr unsigned long long lcg_next(unsigned long long state) {
    return state * 6364136223846793005ULL + 1442695040888963407ULL;
}

template <typename Tp>
constexpr Tp pollard_rho(Tp n) {
    if (n <= 1) return n;
    if (n % 2 == 0) return 2;
    if (alg::math::is_prime(n)) return n;

    Tp x = 2, y = 2, d = 1, c = 1;
    Tp p = 1, l = 1;
    unsigned long long state = static_cast<unsigned long long>(n) + 1337;

    while (d == 1) {
        if (p == l) {
            x = y;
            p <<= 1;
            l = 0;
        }
        y = prho_f(y, c, n);
        ++l;

        Tp diff = x > y ? x - y : y - x;
        d = std::gcd(diff, n);

        if (d == n) {
            state = lcg_next(state);
            x = y = static_cast<Tp>((state % static_cast<unsigned long long>(n - 2)) + 2);
            state = lcg_next(state);
            c = static_cast<Tp>((state % static_cast<unsigned long long>(n - 1)) + 1);
            p = 1; l = 1; d = 1;
        }
    }
    return d;
}

template <typename Tp>
constexpr void recursive_factorize(Tp n, std::vector<Tp>& fact) {
    if (n <= 1) return;
    if (alg::math::is_prime(n)) {
        fact.push_back(n);
        return;
    }
    Tp divs = pollard_rho(n);
    recursive_factorize(divs, fact);
    recursive_factorize(n / divs, fact);
}

} // namespace internal

namespace alg {
namespace math {

template <typename Tp>
constexpr std::vector<std::pair<Tp, int>> factorize(Tp n) {
    std::vector<Tp> fact;
    for (Tp p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}) {
        if (p * p > n) break;
        while (n % p == 0) {
            fact.push_back(p);
            n /= p;
        }
    }
    if (n > 1) {
        internal::recursive_factorize(n, fact);
    }
    std::sort(fact.begin(), fact.end());

    std::vector<std::pair<Tp, int>> ans;
    for (auto it = fact.begin(); it != fact.end();) {
        Tp val = *it;
        auto next = std::upper_bound(it, fact.end(), val);
        ans.emplace_back(val, static_cast<int>(next - it));
        it = next;
    }
    return ans;
}

} // namespace math
} // namespace alg
} // namespace cp
#endif