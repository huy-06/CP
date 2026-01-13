#include <cassert>
#include <iostream>
#include "../../../alg/math/modular/inverse.hpp"
#include "../../../alg/math/number_theory/is_prime.hpp"

#ifndef CP_DS_MODULAR_STATIC_MOD_INT
#define CP_DS_MODULAR_STATIC_MOD_INT
namespace cp {
namespace ds {

#if __cplusplus >= 202002L
template <auto MOD>
class static_mod_int {
public:
    using underlying_type = std::decay_t<decltype(MOD)>;
    using value_type = std::make_unsigned_t<underlying_type>;

    static_assert(std::is_integral_v<underlying_type>, "MOD must be an integral type");
    static_assert(MOD > 0, "MOD must be positive");

    constexpr static_mod_int() noexcept : value(0) {}

    template <std::unsigned_integral U>
    constexpr static_mod_int(U x) : value(x % mod()) {}

    template <std::signed_integral S>
    constexpr static_mod_int(S x) {
        value = alg::mod::safe_mod(x, mod());
    }
#else
template <unsigned long long MOD>
class static_mod_int {
public:
    using value_type = unsigned long long;
    static_assert(MOD > 0, "MOD must be positive");

    constexpr static_mod_int() noexcept : value(0) {}

    constexpr static_mod_int(long long x) {
        value = alg::mod::safe_mod(x, mod());
    }
#endif 

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
        if constexpr (is_mod_prime) {
            return pow(mod() - 2);
        } else {
            value_type g, inv;
            std::tie(g, inv) = alg::mod::inverse(value, mod());
            assert(g == 1);
            return static_mod_int(inv);
        }
    }

    constexpr static_mod_int operator+() const noexcept {
        return *this;
    }

    constexpr static_mod_int operator-() const noexcept {
        return static_mod_int(value ? mod() - value : 0);
    }

    constexpr static_mod_int& operator+=(const static_mod_int& rhs) noexcept {
        value += rhs.val();
        if (value >= mod()) value -= mod();
        return *this;
    }
    
    constexpr static_mod_int& operator-=(const static_mod_int& rhs) noexcept {
        value = value >= rhs.val() ? value - rhs.val() : value + mod() - rhs.val();
        return *this;
    }
    
    constexpr static_mod_int& operator*=(const static_mod_int& rhs) noexcept {
        value = alg::mod::safe_mul(value, rhs.val(), mod());
        return *this;
    }
    
    constexpr static_mod_int& operator/=(const static_mod_int& rhs) noexcept {
        return *this *= rhs.inv();
    }

    constexpr static_mod_int& operator++() noexcept {
        return *this += 1;
    }

    constexpr static_mod_int& operator--() noexcept {
        return *this -= 1;
    }

    constexpr static_mod_int operator++(int) noexcept {
        static_mod_int res = *this;
        ++*this;
        return res;
    }

    constexpr static_mod_int operator--(int) noexcept {
        static_mod_int res = *this;
        --*this;
        return res;
    }

    friend constexpr static_mod_int operator+(static_mod_int lhs, const static_mod_int& rhs) noexcept {
        return lhs += rhs;
    }

    friend constexpr static_mod_int operator-(static_mod_int lhs, const static_mod_int& rhs) noexcept {
        return lhs -= rhs;
    }

    friend constexpr static_mod_int operator*(static_mod_int lhs, const static_mod_int& rhs) noexcept {
        return lhs *= rhs;
    }

    friend constexpr static_mod_int operator/(static_mod_int lhs, const static_mod_int& rhs) noexcept {
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
#if __cplusplus >= 202002L
    static constexpr bool is_mod_prime = alg::is_prime(mod());
#else
    bool is_mod_prime = false;
#endif
};

using mint998244353  = ds::static_mod_int<998244353>;
using mint1000000007 = ds::static_mod_int<1000000007>;

#if __cplusplus >= 202002L

template <typename Tp>
struct is_static_mod_int : std::false_type {};

template <auto MOD>
struct is_static_mod_int<static_mod_int<MOD>> : std::true_type {};

template <typename Tp>
inline constexpr bool is_static_mod_int_v = is_static_mod_int<Tp>::value;

template <typename Tp>
concept modular = is_static_mod_int_v<Tp>;

#endif

} // namespace ds
} // namespace cp
#endif