#include <type_traits>
#include <cassert>
#include "../../../alg/math/modular/pow_mod.hpp"
#include "../../../alg/math/modular/inverse.hpp"

#ifndef CP_DS_MODULAR_DYNAMIC_MOD_INT
#define CP_DS_MODULAR_DYNAMIC_MOD_INT
namespace cp {
namespace ds {

template <typename Tp, int Id>
class dynamic_mod_int {
public:
    using value_type = std::make_unsigned_t<Tp>;

    dynamic_mod_int() noexcept : value(0) {}

#if __cplusplus >= 201402L
    template<std::unsigned_integral U>
    dynamic_mod_int(const U& x) : value(x % mod()) {}

    template<std::signed_integral S>
    dynamic_mod_int(S x) {
        value = alg::mod::safe_mod(x, mod());
    }
#else
    dynamic_mod_int(long long x) {
        value = alg::mod::safe_mod(x, mod());
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

    value_type val() const noexcept {
        return value;
    }

    explicit operator value_type() const noexcept {
        return value;
    }

    template<typename Int>
    dynamic_mod_int pow(Int k) const noexcept {
        return alg::mod::pow_mod(value, k, mod());
    }

    dynamic_mod_int inv() const noexcept {
        value_type g, inv;
        std::tie(g, inv) = alg::mod::inverse(value, mod());
        assert(g == 1);
        return dynamic_mod_int(inv);
    }

    dynamic_mod_int operator+() const noexcept {
        return *this;
    }

    dynamic_mod_int operator-() const noexcept {
        return dynamic_mod_int(value ? mod() - value : 0);
    }

    dynamic_mod_int& operator+=(const dynamic_mod_int& rhs) noexcept {
        value += rhs.val();
        if (value >= mod()) value -= mod();
        return *this;
    }

    dynamic_mod_int& operator-=(const dynamic_mod_int& rhs) noexcept {
        value = value >= rhs.val() ? value - rhs.val() : value + mod() - rhs.val();
        return *this;
    }

    dynamic_mod_int& operator*=(const dynamic_mod_int& rhs) noexcept {
        value = alg::mod::safe_mul(value, rhs.val(), mod());
        return *this;
    }

    dynamic_mod_int& operator/=(const dynamic_mod_int& rhs) noexcept {
        return *this *= rhs.inv();
    }

    dynamic_mod_int& operator++() noexcept {
        return *this += 1;
    }

    dynamic_mod_int& operator--() noexcept {
        return *this -= 1;
    }

    dynamic_mod_int operator++(int) noexcept {
        dynamic_mod_int res = *this;
        ++*this;
        return res;
    }

    dynamic_mod_int operator--(int) noexcept {
        dynamic_mod_int res = *this;
        --*this;
        return res;
    }

    friend dynamic_mod_int operator+(dynamic_mod_int lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs += rhs;
    }

    friend dynamic_mod_int operator-(dynamic_mod_int lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs -= rhs;
    }

    friend dynamic_mod_int operator*(dynamic_mod_int lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs *= rhs;
    }

    friend dynamic_mod_int operator/(dynamic_mod_int lhs, const dynamic_mod_int& rhs) noexcept {
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

    friend bool operator==(const dynamic_mod_int& lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs.val() == rhs.val();
    }

    friend bool operator!=(const dynamic_mod_int& lhs, const dynamic_mod_int& rhs) noexcept {
        return lhs.val() != rhs.val();
    }

private:
    value_type value;
    static inline value_type modular;
};

} // namespace ds
} // namespace cp
#endif