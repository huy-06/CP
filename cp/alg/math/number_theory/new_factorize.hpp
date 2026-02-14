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
Tp pollard_brent(Tp n) {
    if (n % 2 == 0) return 2;

    static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    
    std::uniform_int_distribution<unsigned long long> dist(1, static_cast<unsigned long long>(n) - 1);

    Tp y = static_cast<Tp>(dist(rng));
    Tp c = static_cast<Tp>(dist(rng));
    Tp m = static_cast<Tp>(dist(rng));
    Tp g = 1, r = 1, q = 1;
    Tp x, ys;

    while (g == 1) {
        x = y;
        for (int i = 0; i < r; ++i) 
            y = (alg::mod::safe_mul(y, y, n) + c) % n;
        
        Tp k = 0;
        while (k < r && g == 1) {
            ys = y;
            for (int i = 0; i < std::min(m, r - k); ++i) {
                y = (alg::mod::safe_mul(y, y, n) + c) % n;
                q = alg::mod::safe_mul(q, (x > y) ? (x - y) : (y - x), n);
            }
            g = std::gcd(q, n);
            k += m;
        }
        r *= 2;
    }
    
    if (g == n) {
        do {
            ys = (alg::mod::safe_mul(ys, ys, n) + c) % n;
            g = std::gcd((x > ys) ? (x - ys) : (ys - x), n);
        } while (g == 1);
    }
    return g;
}

template <typename Tp>
void recursive_factorize(Tp n, std::vector<Tp>& factors) {
    if (n == 1) return;
    if (alg::is_prime(n)) {
        factors.push_back(n);
        return;
    }
    Tp divisor = pollard_brent(n);
    recursive_factorize(divisor, factors);
    recursive_factorize(n / divisor, factors);
}


} // namespace internal

namespace alg {

template <typename Tp>
std::vector<std::pair<Tp, int>> factorize(Tp n) {
    std::vector<Tp> raw_factors;
    
    for (Tp p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
        if (p * p > n && n > 1 && raw_factors.empty()) 
            break;

        while (n % p == 0) {
            raw_factors.push_back((Tp)p);
            n /= p;
        }
    }
    
    if (n > 1) {
        internal::recursive_factorize(n, raw_factors);
    }
    
    std::sort(raw_factors.begin(), raw_factors.end());

    std::vector<std::pair<Tp, int>> result;
    if (raw_factors.empty()) 
        return result;

    result.push_back({raw_factors[0], 1});
    for (size_t i = 1; i < raw_factors.size(); ++i) {
        if (raw_factors[i] == result.back().first) {
            result.back().second++;
        } else {
            result.push_back({raw_factors[i], 1});
        }
    }

    return result;
}

} // namespace alg
} // namespace cp
#endif