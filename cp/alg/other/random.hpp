#include <random>
#include <chrono>
#include <type_traits>
#include <limits>
#include <cassert>
#include <vector>
#include <string>

#ifndef CP_ALG_RANDOM
#define CP_ALG_RANDOM
namespace cp {
namespace random {

inline uint64_t splitmix64(uint64_t &x) noexcept {
    uint64_t z = (x += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

inline thread_local std::mt19937_64 rng_inst([]{
    uint64_t seed = (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count();
    uint64_t addr = reinterpret_cast<uint64_t>(&seed);
    seed ^= (addr << 13);
    uint64_t x = seed ? seed : 0x9E3779B97F4A7C15ULL;
    return splitmix64(x);
}());

inline uint64_t rand_u64_raw() noexcept { return rng_inst(); }

inline uint64_t bounded_u64(uint64_t bound) noexcept {
    uint64_t r = rand_u64_raw();
    return static_cast<uint64_t>((__uint128_t)r * bound >> 64);
}

template <typename Tp>
Tp range(Tp l = 0, Tp r = 1000000) {
    static_assert(std::is_arithmetic_v<Tp>, "range requires arithmetic type");
    if constexpr (std::is_integral_v<Tp>) {
        using U = std::make_unsigned_t<Tp>;
        U L = static_cast<U>(l);
        U R = static_cast<U>(r);
        if (L > R) std::swap(L, R);
        U len = R - L + 1;
        U v = static_cast<U>(bounded_u64(static_cast<uint64_t>(len)));
        return static_cast<Tp>(L + v);
    } else {
        uint64_t top53 = rand_u64_raw() >> 11;
        double unit = (double)top53 * (1.0 / 9007199254740992.0);
        double res = static_cast<double>(l) + unit * (static_cast<double>(r) - static_cast<double>(l));
        return static_cast<Tp>(res);
    }
}

inline char choice(const std::string &s) {
    assert(!s.empty());
    return s[static_cast<size_t>(range<int>(0, (int)s.size() - 1))];
}

template<typename Tp>
Tp choice(const std::vector<Tp>& v) {
    assert(!v.empty());
    return v[static_cast<size_t>(range<int>(0, (int)v.size() - 1))];
}

template<typename Tp>
Tp choice(std::initializer_list<Tp> l) {
    std::vector<Tp> tmp(l);
    return choice(tmp);
}

inline std::string string(int n) {
    std::string s;
    s.reserve(n);
    for (int i = 0; i < n; ++i) s.push_back(char('a' + range<int>(0, 25)));
    return s;
}

template<typename Fun>
auto make_vector(int n, Fun f) {
    using Tp = decltype(f());
    std::vector<Tp> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i) v.push_back(f());
    return v;
}

template<typename Tp>
std::vector<Tp> make_vector(int n, Tp l = 0, Tp r = 100000) {
    std::vector<Tp> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i) v.push_back(range<Tp>(l, r));
    return v;
}

inline void seed(uint64_t s) {
    uint64_t x = s ? s : (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count();
    uint64_t s0 = x;
    rng_inst.seed(splitmix64(s0));
}

} // namespace random
} // namespace cp

#endif
