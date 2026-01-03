#include <cassert>
#include <iostream>
#include "../../../alg/math/modular/pow_mod.hpp"
#include "../../../alg/math/modular/inverse.hpp"

#ifndef CP_DS_MODULAR_MOD_INT_61
#define CP_DS_MODULAR_MOD_INT_61
namespace cp {
namespace ds {

#if __cplusplus >= 202002L
class mod_int_61 {
public:
    using value_type = unsigned long long;

    constexpr mod_int_61() noexcept : value(0) {}

    template <std::unsigned_integral U>
    constexpr mod_int_61(U x) : value(x % mod()) {}

    template <std::signed_integral S>
    constexpr mod_int_61(S x) {
        value = alg::mod::safe_mod(x, mod());
    }
#else
class mod_int_61 {
public:
    using value_type = unsigned long long;

    constexpr mod_int_61() noexcept : value(0) {}

    constexpr mod_int_61(long long x) {
        value = alg::mod::safe_mod(x, mod());
    }
#endif 

    static constexpr value_type mod() noexcept {
        return (1ull << 61) - 1;
    }

    constexpr value_type val() const noexcept {
        return value;
    }

    explicit constexpr operator value_type() const noexcept {
        return value;
    }

    template <typename Int>
    constexpr mod_int_61 pow(Int k) const noexcept {
        return alg::mod::pow_mod(value, k, mod());
    }

    constexpr mod_int_61 inv() const noexcept {
        return pow(mod() - 2);
    }

    constexpr mod_int_61 operator+() const noexcept {
        return *this;
    }

    constexpr mod_int_61 operator-() const noexcept {
        return mod_int_61(value ? mod() - value : 0);
    }

    constexpr mod_int_61& operator+=(const mod_int_61& rhs) noexcept {
        value += rhs.val();
        if (value >= mod()) value -= mod();
        return *this;
    }
    
    constexpr mod_int_61& operator-=(const mod_int_61& rhs) noexcept {
        value = value >= rhs.val() ? value - rhs.val() : value + mod() - rhs.val();
        return *this;
    }
    
    constexpr mod_int_61& operator*=(const mod_int_61& rhs) noexcept {
        __int128_t res = __int128_t(value) * rhs.val();
        res = (res >> 61) + (res & mod());
        if (res >= mod()) res -= mod();
        value = res;
        return *this;
    }
    
    constexpr mod_int_61& operator/=(const mod_int_61& rhs) noexcept {
        return *this *= rhs.inv();
    }

    constexpr mod_int_61& operator++() noexcept {
        return *this += 1;
    }

    constexpr mod_int_61& operator--() noexcept {
        return *this -= 1;
    }

    constexpr mod_int_61 operator++(int) noexcept {
        mod_int_61 res = *this;
        ++*this;
        return res;
    }

    constexpr mod_int_61 operator--(int) noexcept {
        mod_int_61 res = *this;
        --*this;
        return res;
    }

    friend constexpr mod_int_61 operator+(mod_int_61 lhs, const mod_int_61& rhs) noexcept {
        return lhs += rhs;
    }

    friend constexpr mod_int_61 operator-(mod_int_61 lhs, const mod_int_61& rhs) noexcept {
        return lhs -= rhs;
    }

    friend constexpr mod_int_61 operator*(mod_int_61 lhs, const mod_int_61& rhs) noexcept {
        return lhs *= rhs;
    }

    friend constexpr mod_int_61 operator/(mod_int_61 lhs, const mod_int_61& rhs) noexcept {
        return lhs /= rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const mod_int_61& m) {
        return os << m.val();
    }

    friend std::istream& operator>>(std::istream& is, mod_int_61& m) {
        long long x;
        is >> x;
        m = x;
        return is;
    }

    friend constexpr bool operator==(const mod_int_61& lhs, const mod_int_61& rhs) noexcept {
        return lhs.val() == rhs.val();
    }

    friend constexpr bool operator!=(const mod_int_61& lhs, const mod_int_61& rhs) noexcept {
        return lhs.val() != rhs.val();
    }

    friend constexpr bool operator<(const mod_int_61& lhs, const mod_int_61& rhs) noexcept {
        return lhs.val() < rhs.val();
    }

    friend constexpr bool operator>(const mod_int_61& lhs, const mod_int_61& rhs) noexcept {
        return lhs.val() > rhs.val();
    }
    friend constexpr bool operator<=(const mod_int_61& lhs, const mod_int_61& rhs) noexcept {
        return lhs.val() <= rhs.val();
    }

    friend constexpr bool operator>=(const mod_int_61& lhs, const mod_int_61& rhs) noexcept {
        return lhs.val() >= rhs.val();
    }

private:
    value_type value;
};

} // namespace ds
} // namespace cp
#endif