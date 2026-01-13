#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include "is_prime.hpp"

#ifndef CP_ALG_MATH_NUM_PRIME
#define CP_ALG_MATH_NUM_PRIME
namespace cp {

namespace internal {

template<typename Tp>
Tp pollard_brent(Tp n) {
    if (n % 2 == 0) return 2;
    if (n % 3 == 0) return 3;
    if (n % 5 == 0) return 5;
    static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<Tp> dist(1, n - 1);
    Tp y = dist(rng);
    Tp c = dist(rng);
    Tp m = dist(rng);
    Tp g = 1, r = 1, q = 1;
    Tp x, ys;
    while (g == 1) {
        x = y;
        for (int i = 0; i < r; ++i)
            y = (mod::mul(y, y, n) + c) % n;
        Tp k = 0;
        while (k < r && g == 1) {
            ys = y;
            for (int i = 0; i < std::min(m, r - k); ++i) {
                y = (mod::mul(y, y, n) + c) % n;
                q = mod::mul(q, (x > y) ? (x - y) : (y - x), n);
            }
            g = std::gcd(q, n);
            k += m;
        }
        r *= 2;
    }
    if (g == n) {
        do {
            ys = (mod::mul(ys, ys, n) + c) % n;
            g = std::gcd((x > ys) ? (x - ys) : (ys - x), n);
        } while (g == 1);
    }
    return g;
}


} // namespace internal

namespace alg {

template<typename Tp>
constexpr std::vector<std::pair<Tp, int>> factorize(Tp n) {
    std::vector<Tp> factors;
    if (n < 2) return {};
    if (n <= 1000000000000ll) {
        while ((n & 1) == 0) {
            factors.push_back(2);
            n >>= 1;
        }
        while (n % 3 == 0) {
            factors.push_back(3);
            n /= 3;
        }
        for (Tp i = 5; i * i <= n; i += 6) {
            while (n % i == 0) {
                factors.push_back(i);
                n /= i;
            }
            Tp j = i + 2;
            while (n % j == 0) {
                factors.push_back(j);
                n /= j;
            }
        }
        if (n > 1) {
            factors.push_back(n);
        }
    } else {
        std::vector<Tp> stk;
        stk.push_back(n);
        while (!stk.empty()) {
            Tp x = stk.back();
            stk.pop_back();
            if (x == 1) continue;
            if (is_prime(x)) {
                factors.push_back(x);
            } else {
                Tp factor = internal::pollard_brent(x);
                stk.push_back(factor);
                stk.push_back(x / factor);
            }
        }
        std::sort(factors.begin(), factors.end());
    }
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