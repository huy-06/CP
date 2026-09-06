#include <cassert>
#include <iostream>
#include "is_modint.hpp"
#include "../../../alg/math/mod/safe_mod.hpp"

#ifndef CP_DS_MODULAR_MONTGOMERY_MOD_INT
#define CP_DS_MODULAR_MONTGOMERY_MOD_INT

namespace cp {
namespace ds {

template <unsigned long long MOD>
class montgomery_mod_int {
public:
    static_assert(MOD > 2 && (MOD & 1), "MOD must be an odd integer > 2");

    using value_type = typename std::conditional<(MOD <= (1ULL << 31)), unsigned int, unsigned long long>::type;
    using double_type = typename std::conditional<(MOD <= (1ULL << 31)), unsigned long long, unsigned __int128>::type;

    constexpr montgomery_mod_int() noexcept : value(0) {}

    template <std::integral Tp>
    constexpr montgomery_mod_int(Tp x) noexcept : value(reduce(static_cast<double_type>(alg::mod::safe_mod(x, mod())) * r2)) {}

    static constexpr montgomery_mod_int raw(value_type v) noexcept {
        montgomery_mod_int x;
        x.value = reduce(static_cast<double_type>(v) * r2);
        return x;
    }

    static constexpr montgomery_mod_int wrap(value_type v) noexcept {
        montgomery_mod_int x;
        x.value = v;
        return x;
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

    constexpr value_type repr() const noexcept {
        return value;
    }

    template <typename Int>
    constexpr montgomery_mod_int pow(Int k) const noexcept {
        if constexpr (std::is_signed_v<Int>) {
            if (k < 0) {
                return inv().pow(0ULL - static_cast<unsigned long long>(k));
            }
        }
        return wrap(pow(value, static_cast<unsigned long long>(k)));
    }

    constexpr montgomery_mod_int inv() const noexcept {
        return wrap(pow(value, mod() - 2));
    }

    constexpr montgomery_mod_int operator+() const noexcept {
        return *this;
    }

    constexpr montgomery_mod_int operator-() const noexcept {
        if (value == 0) return wrap(0);
        return wrap(mod() - value);
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
        value += r1;
        if (value >= mod()) value -= mod();
        return *this;
    }

    constexpr montgomery_mod_int& operator--() noexcept {
        if (value < r1) value += mod();
        value -= r1;
        return *this;
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

    static constexpr value_type neg_inv = []() constexpr {
        value_type inv = MOD;
        inv *= 2 - MOD * inv;
        inv *= 2 - MOD * inv;
        inv *= 2 - MOD * inv;
        inv *= 2 - MOD * inv;
        if constexpr (sizeof(value_type) == 8) inv *= 2 - MOD * inv;
        return -inv;
    }();

    static constexpr value_type r1 = -static_cast<value_type>(MOD) % MOD;

    static constexpr value_type r2 = static_cast<value_type>((static_cast<double_type>(r1) * r1) % MOD);

    static constexpr value_type reduce(double_type x) noexcept {
        value_type m = static_cast<value_type>(x) * neg_inv;
        value_type t = (x + static_cast<double_type>(m) * MOD) >> (sizeof(value_type) * 8);
        return t >= MOD ? t - MOD : t;
    }

    static constexpr value_type pow(value_type base, unsigned long long k) noexcept {
        value_type res = r1;
        value_type a = base;
        while (k > 0) {
            if (k & 1) {
                res = reduce(static_cast<double_type>(res) * a);
            }
            a = reduce(static_cast<double_type>(a) * a);
            k >>= 1;
        }
        return res;
    }
};

using mont998244353 = montgomery_mod_int<998244353>;
using mont1000000007 = montgomery_mod_int<1000000007>;
using mont2_61_1 = montgomery_mod_int<(1ULL << 61) - 1>;

template <unsigned long long MOD>
inline constexpr bool is_modint_v<montgomery_mod_int<MOD>> = true;

} // namespace ds
} // namespace cp

namespace std {

template <unsigned long long MOD>
struct hash<cp::ds::montgomery_mod_int<MOD>> {
    size_t operator()(const cp::ds::montgomery_mod_int<MOD>& m) const noexcept {
        using value_type = typename cp::ds::montgomery_mod_int<MOD>::value_type;
        return hash<value_type>{}(m.repr());
    }
};

} // namespace std

#endif