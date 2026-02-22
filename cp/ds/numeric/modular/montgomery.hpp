#include <cassert>
#include <iostream>
#include <utility>
#include <limits>
#include "../../../init/constexpr.hpp"
#include "../../../alg/math/modular/safe_mod.hpp"

#ifndef CP_DS_MODULAR_MONTGOMERY_MOD_INT
#define CP_DS_MODULAR_MONTGOMERY_MOD_INT
namespace cp {
namespace ds {

template <unsigned long long MOD>
class montgomery_mod_int {
private:
    enum class raw_tag { 
        construct
    };

public:
    static_assert(MOD > 2 && (MOD & 1), "MOD must be an odd integer > 2");
    
    using value_type = typename std::conditional<(MOD <= 4294967295ULL), unsigned int, unsigned long long>::type;
    using double_type = typename std::conditional<(MOD <= 4294967295ULL), unsigned long long, unsigned __int128>::type;

    constexpr montgomery_mod_int() noexcept : value(0) {}

    constexpr montgomery_mod_int(value_type v, raw_tag) noexcept : value(v) {}

    template <typename Tp>
    constexpr montgomery_mod_int(Tp x) noexcept : value(reduce(static_cast<double_type>(alg::mod::safe_mod(x, mod())) * r2)) {}

    static constexpr value_type mod() noexcept {
        return MOD;
    }

    constexpr value_type val() const noexcept {
        return reduce(value);
    }

    explicit constexpr operator value_type() const noexcept {
        return val();
    }

    template <typename Int>
    constexpr montgomery_mod_int pow(Int k) const noexcept {
        return k < 0 ? 
            montgomery_mod_int(pow(pow(value, mod() - 2), 0ULL - static_cast<unsigned long long>(k)), raw_tag::construct) : 
            montgomery_mod_int(pow(value, static_cast<unsigned long long>(k)), raw_tag::construct);
    }

    constexpr montgomery_mod_int inv() const noexcept {
        return montgomery_mod_int(pow(value, mod() - 2), raw_tag::construct);
    }

    constexpr montgomery_mod_int operator+() const noexcept {
        return *this;
    }

    constexpr montgomery_mod_int operator-() const noexcept {
        return montgomery_mod_int(value ? mod() - value : 0, raw_tag::construct);
    }

    cp_constexpr montgomery_mod_int& operator+=(const montgomery_mod_int& rhs) noexcept {
        value += rhs.value;
        if (value >= mod()) value -= mod();
        return *this;
    }

    cp_constexpr montgomery_mod_int& operator-=(const montgomery_mod_int& rhs) noexcept {
        if (value < rhs.value) value += mod();
        value -= rhs.value;
        return *this;
    }

    cp_constexpr montgomery_mod_int& operator*=(const montgomery_mod_int& rhs) noexcept {
        value = reduce(static_cast<double_type>(value) * rhs.value);
        return *this;
    }

    cp_constexpr montgomery_mod_int& operator/=(const montgomery_mod_int& rhs) noexcept {
        return *this *= rhs.inv();
    }

    cp_constexpr montgomery_mod_int& operator++() noexcept {
        return *this += 1;
    }

    cp_constexpr montgomery_mod_int& operator--() noexcept {
        return *this -= 1;
    }

    cp_constexpr montgomery_mod_int operator++(int) noexcept {
        montgomery_mod_int res = *this;
        ++*this;
        return res;
    }

    cp_constexpr montgomery_mod_int operator--(int) noexcept {
        montgomery_mod_int res = *this;
        --*this;
        return res;
    }

    friend cp_constexpr montgomery_mod_int operator+(montgomery_mod_int lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs += rhs;
    }

    friend cp_constexpr montgomery_mod_int operator-(montgomery_mod_int lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs -= rhs;
    }

    friend cp_constexpr montgomery_mod_int operator*(montgomery_mod_int lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs *= rhs;
    }

    friend cp_constexpr montgomery_mod_int operator/(montgomery_mod_int lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs /= rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const montgomery_mod_int& m) {
        return os << m.val();
    }

    friend std::istream& operator>>(std::istream& is, montgomery_mod_int& m) {
        long long x;
        is >> x;
        m = x;
        return is;
    }

    friend constexpr bool operator==(const montgomery_mod_int& lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs.value == rhs.value;
    }

    friend constexpr bool operator!=(const montgomery_mod_int& lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs.value != rhs.value;
    }

    friend constexpr bool operator<(const montgomery_mod_int& lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs.val() < rhs.val();
    }

    friend constexpr bool operator>(const montgomery_mod_int& lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs.val() > rhs.val();
    }

    friend constexpr bool operator<=(const montgomery_mod_int& lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs.val() <= rhs.val();
    }

    friend constexpr bool operator>=(const montgomery_mod_int& lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs.val() >= rhs.val();
    }

private:
    value_type value;

    static constexpr value_type calc_neg_inv(value_type inv, int i) {
        return i == 0 ? -inv : calc_neg_inv(static_cast<value_type>(inv * (2 - mod() * inv)), i - 1);
    }

    static constexpr value_type calc_neg_inv() {
        return calc_neg_inv(mod(), 6);
    }

    static constexpr value_type neg_inv = calc_neg_inv();

    static constexpr value_type calc_r2() {
        return static_cast<value_type>((static_cast<double_type>(-mod() % mod()) * (-mod() % mod())) % mod());
    }

    static constexpr value_type r2 = calc_r2();

    static constexpr int bit_width = std::numeric_limits<value_type>::digits;

    static constexpr value_type reduce_impl(double_type t) noexcept {
        return static_cast<value_type>(t >= mod() ? t - mod() : t);
    }

    static constexpr value_type reduce(double_type x) noexcept {
        return reduce_impl((x + static_cast<double_type>(static_cast<value_type>(x) * neg_inv) * mod()) >> bit_width);
    }

    static constexpr value_type pow(value_type base_val, unsigned long long k) noexcept {
        return k == 0 ? reduce(r2) :
               (k & 1) ? reduce(static_cast<double_type>(base_val) * pow(reduce(static_cast<double_type>(base_val) * base_val), k >> 1)) :
               pow(reduce(static_cast<double_type>(base_val) * base_val), k >> 1);
    }

};

template <unsigned long long MOD>
constexpr typename montgomery_mod_int<MOD>::value_type montgomery_mod_int<MOD>::neg_inv;

template <unsigned long long MOD>
constexpr typename montgomery_mod_int<MOD>::value_type montgomery_mod_int<MOD>::r2;

template <unsigned long long MOD>
constexpr int montgomery_mod_int<MOD>::bit_width;

using mont998244353 = montgomery_mod_int<998244353>;
using mont1000000007 = montgomery_mod_int<1000000007>;

} // namespace ds
} // namespace cp
#endif