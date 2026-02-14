#include "../modular/pow_mod.hpp"
// #include "../../../cst/primes.hpp"

#ifndef CP_ALG_IS_PRIME
#define CP_ALG_IS_PRIME
namespace cp {
namespace alg {

constexpr bool is_prime(unsigned n) {
    if (n < 2) 
        return false;
        
    for (unsigned p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
        if (n % p == 0)
            return n == p;
    }

    unsigned d = n - 1;
    int s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        ++s;
    }

    for (unsigned a : {2, 7, 61}) {
        if (a >= n) 
            break;

        unsigned x = mod::pow_mod(a, d, n);
        if (x == 1 || x == n - 1) 
            continue;

        bool composite = true;
        for (int r = 1; r < s; ++r) {
            x = mod::safe_mul(x, x, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        if (composite) 
            return false;
    }

    return true;
}

constexpr bool is_prime(int n) {
    if (n < 2)
        return false;
    return is_prime(static_cast<unsigned>(n));
}

constexpr bool is_prime(unsigned long long n) {
    if (n < 2) 
        return false;

    for (unsigned long long p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
        if (n % p == 0)
            return n == p;
    }

    unsigned long long d = n - 1;
    int s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        ++s;
    }

    for (unsigned long long a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
        if (a >= n) 
            break;

        unsigned long long x = mod::pow_mod(a, d, n);
        if (x == 1 || x == n - 1) 
            continue;

        bool composite = true;
        for (int r = 1; r < s; ++r) {
            x = mod::safe_mul(x, x, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        if (composite) 
            return false;
    }

    return true;
}

constexpr bool is_prime(long long n) {
    if (n < 2)
        return false;
    return is_prime(static_cast<unsigned long long>(n));
}

} // namespace alg
} // namespace cp
#endif