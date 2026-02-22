#include "pow_mod.hpp"

#ifndef CP_ALG_MATH_MOD_PRIME_ROOT
#define CP_ALG_MATH_MOD_PRIME_ROOT
namespace cp {

namespace internal {

constexpr int extract_pow2(int x) {
    return x % 2 == 0 ? extract_pow2(x / 2) : x;
}

constexpr int remove_factor(int x, int i) {
    return x % i == 0 ? remove_factor(x / i, i) : x;
}

constexpr bool check_pow(int g, int m, int p) {
    return alg::mod::pow_mod(g, (m - 1) / p, m) != 1;
}

constexpr bool check_divisors(int g, int m, int x, int i) {
    return static_cast<long long>(i) * i > x ? (x > 1 ? check_pow(g, m, x) : true) :
            (x % i == 0 ? 
                (check_pow(g, m, i) && check_divisors(g, m, remove_factor(x, i), i + 2)) :
                check_divisors(g, m, x, i + 2));
}

constexpr bool is_primitive_root(int g, int m) {
    return check_pow(g, m, 2) && check_divisors(g, m, extract_pow2((m - 1) / 2), 3);
}

constexpr int find_primitive_root(int g, int m) {
    return is_primitive_root(g, m) ? g : find_primitive_root(g + 1, m);
}

}

namespace alg {
namespace mod {

constexpr int primitive_root(int m) {
    return m == 2 ? 1 :
           m == 167772161 ? 3 :
           m == 469762049 ? 3 :
           m == 754974721 ? 11 :
           m == 998244353 ? 3 : 
           internal::find_primitive_root(2, m);
}

} // namespace mod
} // namespace alg
} // namespace cp
#endif
