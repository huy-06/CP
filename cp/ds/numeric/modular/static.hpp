#include <cassert>
#include <iostream>
#include <tuple>
#include "../../../init/constexpr.hpp"
#include "../../../alg/math/modular/inverse.hpp"
#include "../../../alg/math/number_theory/is_prime.hpp"

#ifndef CP_DS_MODULAR_STATIC_MOD_INT
#define CP_DS_MODULAR_STATIC_MOD_INT
namespace cp {
namespace ds {

template <unsigned long long MOD>
class static_mod_int {
public:
    static_assert(MOD > 0, "MOD must be positive");

    using value_type = typename std::conditional<(MOD <= 4294967295ULL), unsigned int, unsigned long long>::type;

    constexpr static_mod_int() noexcept : value(0) {}

    template <typename Tp>
    constexpr static_mod_int(Tp x) noexcept : value(alg::mod::safe_mod(x, mod())) {}

    static constexpr value_type mod() noexcept {
        return static_cast<value_type>(MOD);
    }

    constexpr value_type val() const noexcept {
        return value;
    }

    explicit constexpr operator value_type() const noexcept {
        return value;
    }

    template <typename Int>
    constexpr static_mod_int pow(Int k) const noexcept {
        return alg::mod::pow_mod(value, k, mod());
    }

    constexpr static_mod_int inv() const noexcept {
        return alg::is_prime(MOD) ? pow(MOD - 2) : 
               static_mod_int(alg::mod::inverse(value, mod()).second);
    }

    constexpr static_mod_int operator+() const noexcept {
        return *this;
    }

    constexpr static_mod_int operator-() const noexcept {
        return static_mod_int(value ? mod() - value : 0);
    }

    cp_constexpr static_mod_int& operator+=(const static_mod_int& rhs) noexcept {
        value += rhs.val();
        if (value >= mod()) value -= mod();
        return *this;
    }
    
    cp_constexpr static_mod_int& operator-=(const static_mod_int& rhs) noexcept {
        value = value >= rhs.val() ? value - rhs.val() : value + mod() - rhs.val();
        return *this;
    }
    
    cp_constexpr static_mod_int& operator*=(const static_mod_int& rhs) noexcept {
        value = alg::mod::safe_mul(value, rhs.val(), mod());
        return *this;
    }
    
    cp_constexpr static_mod_int& operator/=(const static_mod_int& rhs) noexcept {
        return *this *= rhs.inv();
    }

    cp_constexpr static_mod_int& operator++() noexcept {
        return *this += 1;
    }

    cp_constexpr static_mod_int& operator--() noexcept {
        return *this -= 1;
    }

    cp_constexpr static_mod_int operator++(int) noexcept {
        static_mod_int res = *this;
        ++*this;
        return res;
    }

    cp_constexpr static_mod_int operator--(int) noexcept {
        static_mod_int res = *this;
        --*this;
        return res;
    }

    friend cp_constexpr static_mod_int operator+(static_mod_int lhs, const static_mod_int& rhs) noexcept {
        return lhs += rhs;
    }

    friend cp_constexpr static_mod_int operator-(static_mod_int lhs, const static_mod_int& rhs) noexcept {
        return lhs -= rhs;
    }

    friend cp_constexpr static_mod_int operator*(static_mod_int lhs, const static_mod_int& rhs) noexcept {
        return lhs *= rhs;
    }

    friend cp_constexpr static_mod_int operator/(static_mod_int lhs, const static_mod_int& rhs) noexcept {
        return lhs /= rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const static_mod_int& m) {
        return os << m.val();
    }

    friend std::istream& operator>>(std::istream& is, static_mod_int& m) {
        long long x;
        is >> x;
        m = x;
        return is;
    }

    friend constexpr bool operator==(const static_mod_int& lhs, const static_mod_int& rhs) noexcept {
        return lhs.val() == rhs.val();
    }

    friend constexpr bool operator!=(const static_mod_int& lhs, const static_mod_int& rhs) noexcept {
        return lhs.val() != rhs.val();
    }

    friend constexpr bool operator<(const static_mod_int& lhs, const static_mod_int& rhs) noexcept {
        return lhs.val() < rhs.val();
    }

    friend constexpr bool operator>(const static_mod_int& lhs, const static_mod_int& rhs) noexcept {
        return lhs.val() > rhs.val();
    }
    friend constexpr bool operator<=(const static_mod_int& lhs, const static_mod_int& rhs) noexcept {
        return lhs.val() <= rhs.val();
    }

    friend constexpr bool operator>=(const static_mod_int& lhs, const static_mod_int& rhs) noexcept {
        return lhs.val() >= rhs.val();
    }

private:
    value_type value;
};

using mint998244353  = ds::static_mod_int<998244353>;
using mint1000000007 = ds::static_mod_int<1000000007>;

} // namespace ds
} // namespace cp
#endif