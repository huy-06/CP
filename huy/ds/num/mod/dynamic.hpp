#include <type_traits>
#include <cassert>
#include "../../../alg/math/mod/exp.hpp"
#include "../../../alg/math/mod/inv_gcd.hpp"

#ifndef HUY_DS_MOD_DYNAMIC
#define HUY_DS_MOD_DYNAMIC
namespace huy {
namespace ds {

template <typename Tp, int Id>
class dynamic_mod_int {
public:
    using value_type = std::make_unsigned_t<Tp>;

    constexpr dynamic_mod_int() noexcept : value(0) {}

#if __cplusplus >= 201402L
    template<std::unsigned_integral U>
    constexpr dynamic_mod_int(const U& x) : value(x % mod()) {}

    template<std::signed_integral S>
    constexpr dynamic_mod_int(S x) {
        value = alg::mod::safe(x, mod());
    }
#else
    constexpr dynamic_mod_int(long long x) {
        value = alg::mod::safe(x, mod());
    }
#endif 

    template <typename U>
    static void set_mod(const U& m) { 
        assert(m > 0);
        modular = static_cast<value_type>(m);
    }

    static value_type mod() noexcept {
        return modular;
    }

    constexpr value_type val() const noexcept {
        return value;
    }

    explicit constexpr operator value_type() const noexcept {
        return value;
    }

    template<typename Int>
    constexpr dynamic_mod_int pow(Int k) const noexcept {
        return alg::mod::exp(value, k, mod());
    }

    constexpr dynamic_mod_int inv() const noexcept {
        value_type g, inv;
        std::tie(g, inv) = alg::mod::inv_gcd(value, mod());
        assert(g == 1);
        return dynamic_mod_int(inv);
    }

    constexpr dynamic_mod_int operator+() const noexcept {
        return *this;
    }

    constexpr dynamic_mod_int operator-() const noexcept {
        return dynamic_mod_int(value ? mod() - value : 0);
    }

    constexpr dynamic_mod_int& operator+=(const dynamic_mod_int& rhs) noexcept {
        value += rhs.val();
        if (value >= mod()) value -= mod();
        return *this;
    }

    constexpr dynamic_mod_int& operator-=(const dynamic_mod_int& rhs) noexcept {
        value = value >= rhs.val() ? value - rhs.val() : value + mod() - rhs.val();
        return *this;
    }

    constexpr dynamic_mod_int& operator*=(const dynamic_mod_int& rhs) noexcept {
        value = alg::mod::mul(value, rhs.val(), mod());
        return *this;
    }

    constexpr dynamic_mod_int& operator/=(const dynamic_mod_int& rhs) noexcept {
        return *this *= rhs.inv();
    }

    constexpr dynamic_mod_int& operator++() noexcept {
        return *this += 1;
    }

    constexpr dynamic_mod_int& operator--() noexcept {
        return *this -= 1;
    }

    constexpr dynamic_mod_int operator++(int) noexcept {
        dynamic_mod_int res = *this;
        ++*this;
        return res;
    }

    constexpr dynamic_mod_int operator--(int) noexcept {
        dynamic_mod_int res = *this;
        --*this;
        return res;
    }

    friend constexpr dynamic_mod_int operator+(dynamic_mod_int lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs += rhs;
    }

    friend constexpr dynamic_mod_int operator-(dynamic_mod_int lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs -= rhs;
    }

    friend constexpr dynamic_mod_int operator*(dynamic_mod_int lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs *= rhs;
    }

    friend constexpr dynamic_mod_int operator/(dynamic_mod_int lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs /= rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const dynamic_mod_int& m) {
        return os << m.value;
    }

    friend std::istream& operator>>(std::istream& is, dynamic_mod_int& m) {
        long long x;
        is >> x;
        m = x;
        return is;
    }

    friend constexpr bool operator==(const dynamic_mod_int& lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs.val() == rhs.val();
    }

    friend constexpr bool operator!=(const dynamic_mod_int& lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs.val() != rhs.val();
    }

private:
    value_type value;
    static value_type modular;
};

template <typename Tp, int Id>
typename dynamic_mod_int<Tp, Id>::value_type dynamic_mod_int<Tp, Id>::modular = 998244353;

template<typename Tp, int Id>
struct is_dynamic_mod_int : std::false_type {};

template<typename Tp, int Id>
struct is_dynamic_mod_int<dynamic_mod_int<Tp, Id>, Id> : std::true_type {};

template<typename Tp, int Id>
constexpr bool is_dynamic_mod_int_v = is_dynamic_mod_int<Tp, Id>::value;

} // namespace ds
} // namespace huy
#endif