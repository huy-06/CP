#include <cassert>
#include <iostream>
#include <utility>
#include <limits>
#include "mod_int_traits.hpp"
#include "../../../alg/math/modular/safe_mod.hpp"

#ifndef CP_DS_MODULAR_MONTGOMERY_MOD_INT
#define CP_DS_MODULAR_MONTGOMERY_MOD_INT
namespace cp {
namespace ds {

template <unsigned long long MOD>
class montgomery_mod_int {
private:
    static_assert(MOD > 2 && (MOD & 1), "MOD must be an odd integer > 2");

    using traits = internal::mod_int_traits<MOD>;

public:
    using value_type = typename traits::value_type;
    using double_type = typename traits::double_type;

    constexpr montgomery_mod_int() noexcept : value(0) {}

    template <typename Tp>
    constexpr montgomery_mod_int(Tp x) noexcept {
        value = reduce(static_cast<double_type>(alg::mod::safe_mod(x, mod())) * R2);
    }

    static constexpr value_type mod() noexcept {
        return MOD;
    }

    constexpr value_type val() const noexcept {
        return reduce(value);
    }

    explicit constexpr operator value_type() const noexcept {
        return val();
    }

    constexpr montgomery_mod_int operator+() const noexcept {
        return *this;
    }

    constexpr montgomery_mod_int operator-() const noexcept {
        montgomery_mod_int res;
        res.value = (value == 0) ? 0 : mod() - value;
        return res;
    }

    constexpr montgomery_mod_int& operator+=(const montgomery_mod_int& rhs) noexcept {
        value += rhs.value;
        if (value >= mod()) value -= mod();
        return *this;
    }

    constexpr montgomery_mod_int& operator-=(const montgomery_mod_int& rhs) noexcept {
        if (value < rhs.value) value += mod();
        value -= rhs.value;
        return *this;
    }

    constexpr montgomery_mod_int& operator*=(const montgomery_mod_int& rhs) noexcept {
        value = reduce(static_cast<double_type>(value) * rhs.value);
        return *this;
    }

    constexpr montgomery_mod_int& operator/=(const montgomery_mod_int& rhs) noexcept {
        return *this *= rhs.inv();
    }

    constexpr montgomery_mod_int& operator++() noexcept {
        return *this += 1;
    }

    constexpr montgomery_mod_int& operator--() noexcept {
        return *this -= 1;
    }

    constexpr montgomery_mod_int operator++(int) noexcept {
        montgomery_mod_int res = *this;
        ++*this;
        return res;
    }

    constexpr montgomery_mod_int operator--(int) noexcept {
        montgomery_mod_int res = *this;
        --*this;
        return res;
    }

    friend constexpr montgomery_mod_int operator+(montgomery_mod_int lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs += rhs;
    }

    friend constexpr montgomery_mod_int operator-(montgomery_mod_int lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs -= rhs;
    }

    friend constexpr montgomery_mod_int operator*(montgomery_mod_int lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs *= rhs;
    }

    friend constexpr montgomery_mod_int operator/(montgomery_mod_int lhs, const montgomery_mod_int& rhs) noexcept {
        return lhs /= rhs;
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

    template <typename Int>
    constexpr montgomery_mod_int pow(Int k) const noexcept {
        montgomery_mod_int res(1);
        montgomery_mod_int base = *this;

        if (k < 0) {
            base = base.inv();
            k = -k;
        }

        for (; k > 0; k >>= 1, base *= base) {
            if (k & 1) res *= base;
        }

        return res;
    }

    constexpr montgomery_mod_int inv() const noexcept {
        return pow(mod() - 2);
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

private:
    value_type value;

    static constexpr int BIT_WIDTH = std::numeric_limits<value_type>::digits;

    static constexpr value_type logic_neg_inv() {
        value_type inv = mod();
        for (int i = 0; i < 6; ++i) 
            inv *= 2 - mod() * inv;
        return -inv;
    }

    static constexpr value_type logic_r2() {
        value_type r = -mod() % mod(); 
        return static_cast<value_type>((static_cast<double_type>(r) * r) % mod());
    }

    static constexpr value_type NEG_INV = logic_neg_inv();
    static constexpr value_type R2      = logic_r2();

    static constexpr value_type reduce(double_type T) noexcept {
        value_type m = static_cast<value_type>(T) * NEG_INV;
        double_type t = (T + static_cast<double_type>(m) * mod()) >> BIT_WIDTH;
        if (t >= mod()) t -= mod();
        return static_cast<value_type>(t);
    }
};

using mont998244353 = montgomery_mod_int<998244353>;
using mont1000000007 = montgomery_mod_int<1000000007>;

} // namespace ds
} // namespace cp
#endif