#include "../modular/pow_mod.hpp"
// #include "../../../cst/primes.hpp"

#ifndef CP_ALG_IS_PRIME
#define CP_ALG_IS_PRIME
namespace cp {

namespace internal {
    
constexpr int get_s(unsigned long long d, int s = 0) {
    return (d & 1) == 0 ? get_s(d >> 1, s + 1) : s;
}

constexpr unsigned long long get_d(unsigned long long d) {
    return (d & 1) == 0 ? get_d(d >> 1) : d;
}

constexpr bool mr_loop(unsigned long long x, unsigned long long n, int r, int s) {
    return r == s ? false :
            (x == n - 1 ? true :
            mr_loop(alg::mod::safe_mul(x, x, n), n, r + 1, s));
}

constexpr bool mr_test(unsigned long long a, unsigned long long n, unsigned long long d, int s) {
    return a >= n ? true :
            (alg::mod::pow_mod(a, d, n) == 1 || alg::mod::pow_mod(a, d, n) == n - 1 ? true :
            mr_loop(alg::mod::safe_mul(alg::mod::pow_mod(a, d, n), alg::mod::pow_mod(a, d, n), n), n, 1, s));
}

constexpr bool chk_32(unsigned n, unsigned d, int s, int idx) {
    return idx == 0 ? mr_test(2, n, d, s) && chk_32(n, d, s, 1) :
            idx == 1 ? mr_test(7, n, d, s) && chk_32(n, d, s, 2) :
            idx == 2 ? mr_test(61, n, d, s) : true;
}

constexpr bool chk_64(unsigned long long n, unsigned long long d, int s, int idx) {
    return idx == 0 ? mr_test(2, n, d, s) && chk_64(n, d, s, 1) :
            idx == 1 ? mr_test(325, n, d, s) && chk_64(n, d, s, 2) :
            idx == 2 ? mr_test(9375, n, d, s) && chk_64(n, d, s, 3) :
            idx == 3 ? mr_test(28178, n, d, s) && chk_64(n, d, s, 4) :
            idx == 4 ? mr_test(450775, n, d, s) && chk_64(n, d, s, 5) :
            idx == 5 ? mr_test(9780504, n, d, s) && chk_64(n, d, s, 6) :
            idx == 6 ? mr_test(1795265022, n, d, s) : true;
}

constexpr bool is_prime_32(unsigned n) {
    return n < 2 ? false : (n % 2 == 0 ? n == 2 : (n % 3 == 0 ? n == 3 : (n % 5 == 0 ? n == 5 : 
            (n % 7 == 0 ? n == 7 : (n % 11 == 0 ? n == 11 : (n % 13 == 0 ? n == 13 : 
            (n % 17 == 0 ? n == 17 : (n % 19 == 0 ? n == 19 : (n % 23 == 0 ? n == 23 : 
            (n % 29 == 0 ? n == 29 : (n % 31 == 0 ? n == 31 : (n % 37 == 0 ? n == 37 : 
            chk_32(n, get_d(n - 1), get_s(n - 1), 0)))))))))))));
}

constexpr bool is_prime_64(unsigned long long n) {
    return n < 2 ? false : (n % 2 == 0 ? n == 2 : (n % 3 == 0 ? n == 3 : (n % 5 == 0 ? n == 5 : 
            (n % 7 == 0 ? n == 7 : (n % 11 == 0 ? n == 11 : (n % 13 == 0 ? n == 13 : 
            (n % 17 == 0 ? n == 17 : (n % 19 == 0 ? n == 19 : (n % 23 == 0 ? n == 23 : 
            (n % 29 == 0 ? n == 29 : (n % 31 == 0 ? n == 31 : (n % 37 == 0 ? n == 37 : 
            chk_64(n, get_d(n - 1), get_s(n - 1), 0)))))))))))));
}

} // namespace internal

namespace alg {

constexpr bool is_prime(unsigned n) { 
    return internal::is_prime_32(n);
}

constexpr bool is_prime(int n) {
    return n < 2 ? false : internal::is_prime_32(static_cast<unsigned>(n));
}

constexpr bool is_prime(unsigned long long n) {
    return internal::is_prime_64(n);
}

constexpr bool is_prime(long long n) {
    return n < 2 ? false : internal::is_prime_64(static_cast<unsigned long long>(n));
}

} // namespace alg
} // namespace cp
#endif