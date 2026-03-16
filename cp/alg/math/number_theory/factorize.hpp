#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include "is_prime.hpp"
#include "../../../init/constexpr.hpp"

#ifndef CP_ALG_MATH_NUM_PRIME
#define CP_ALG_MATH_NUM_PRIME
namespace cp {

namespace internal {

template <typename Tp>
cp_constexpr Tp prho_f(Tp x, Tp c, Tp n) {
    using unsigned_type = typename std::make_unsigned<Tp>::type;
    Tp res = static_cast<Tp>(alg::mod::safe_mul(
        static_cast<unsigned_type>(x), 
        static_cast<unsigned_type>(x), 
        static_cast<unsigned_type>(n)
    )) + c;
    return res >= n ? res - n : res;
}

constexpr unsigned long long lcg_next(unsigned long long state) {
    return state * 6364136223846793005ULL + 1442695040888963407ULL;
}

template <typename Tp>
cp_constexpr Tp pollard_rho(Tp n) {
    if (n % 2 == 0) return 2;
    if (alg::is_prime(n)) return n;

    Tp x = 2, y = 2, d = 1, c = 1;
    Tp power = 1, lam = 1;
    unsigned long long state = static_cast<unsigned long long>(n) + 1337;

    while (d == 1) {
        if (power == lam) {
            x = y;
            power <<= 1;
            lam = 0;
        }
        y = prho_f(y, c, n);
        lam++;
        
        Tp diff = x > y ? x - y : y - x;
        d = std::gcd(diff, n);

        if (d == n) {
            state = lcg_next(state);
            x = y = static_cast<Tp>((state % (n - 2)) + 2);
            state = lcg_next(state);
            c = static_cast<Tp>((state % (n - 1)) + 1);
            power = 1; lam = 1; d = 1;
        }
    }
    return d;
}

template <typename Tp>
cp_constexpr void recursive_factorize(Tp n, std::vector<Tp>& factors) {
    if (n <= 1) return;
    if (alg::is_prime(n)) {
        factors.push_back(n);
        return;
    }
    Tp divisor = pollard_rho(n);
    recursive_factorize(divisor, factors);
    recursive_factorize(n / divisor, factors);
}

} // namespace internal

namespace alg {

template <typename Tp>
cp_constexpr std::vector<std::pair<Tp, int>> factorize(Tp n) {
    std::vector<Tp> factors;
    for (Tp p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}) {
        if (p * p > n) break;
        while (n % p == 0) {
            factors.push_back(p);
            n /= p;
        }
    }
    if (n > 1) {
        internal::recursive_factorize(n, factors);
    }
    std::sort(factors.begin(), factors.end());

    std::vector<std::pair<Tp, int>> ans;
    for (auto it = factors.begin(); it != factors.end();) {
        Tp val = *it;
        auto next = std::upper_bound(it, factors.end(), val);
        ans.emplace_back(val, next - it);
        it = next;
    }
    return ans;
}

} // namespace alg
} // namespace cp
#endif