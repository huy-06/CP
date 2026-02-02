#include <iostream>
#include <iomanip>
#include "../../../init/constexpr.hpp"
#include "../../../alg/math/arithmetic/arithmetic.hpp"

#ifndef CP_DS_REAL
#define CP_DS_REAL
namespace cp {
namespace ds {

#if __cplusplus >= 202002L
template <auto EPS>
#else
template <long double EPS>
#endif
class real {
public:
    using value_type = std::decay_t<decltype(EPS)>;

    static_assert(std::is_floating_point_v<value_type>, "EPS must be a floating point number");

    constexpr real() noexcept 
        : value(0) {}

    template <typename Tp> 
    constexpr real(const Tp& x) noexcept 
        : value(static_cast<value_type>(x)) {}

    static constexpr value_type eps() noexcept {
        return EPS;
    }

    constexpr value_type val() const noexcept {
        return value;
    }

    explicit constexpr operator value_type() const noexcept {
        return value;
    }

    static constexpr int precision() noexcept {
        value_type e = eps();
        if (e >= static_cast<value_type>(1.0)) {
            return 0;
        }

        int p = 0;
        while (e < static_cast<value_type>(0.99)) {
            e *= 10;
            ++p;
        }

        return p;
    }

    constexpr real inv() const {
        return real(static_cast<value_type>(1.0)) / *this;
    }

    template <typename Tp>
    constexpr auto pow(const Tp& exp) const noexcept -> typename std::enable_if<std::is_arithmetic<Tp>::value, real>::type {
        return real(alg::pow(value, exp));
    }

    constexpr real pow(const real& exp) const noexcept {
        return real(alg::pow(value, exp.value));
    }

    constexpr real operator+() const noexcept { 
        return *this; 
    }

    constexpr real operator-() const noexcept {
        return real(-value);
    }

    constexpr real& operator+=(const real& rhs) noexcept {
        value += rhs.value;
        return *this;
    }

    constexpr real& operator-=(const real& rhs) noexcept {
        value -= rhs.value;
        return *this;
    }

    constexpr real& operator*=(const real& rhs) noexcept {
        value *= rhs.value;
        return *this;
    }

    constexpr real& operator/=(const real& rhs) noexcept {
        value /= rhs.value;
        return *this;
    }

    friend constexpr real operator+(real lhs, const real& rhs) noexcept { 
        return lhs += rhs;
    }

    friend constexpr real operator-(real lhs, const real& rhs) noexcept {
        return lhs -= rhs;
    }

    friend constexpr real operator*(real lhs, const real& rhs) noexcept {
        return lhs *= rhs;
    }

    friend constexpr real operator/(real lhs, const real& rhs) noexcept {
        return lhs /= rhs;
    }
    
    friend constexpr bool operator==(const real& lhs, const real& rhs) noexcept {
        auto diff = lhs.value - rhs.value;
        return (diff < 0 ? -diff : diff) <= eps();
    }

    friend constexpr bool operator!=(const real& lhs, const real& rhs) noexcept {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const real& lhs, const real& rhs) noexcept {
        return lhs.value < rhs.value - eps();
    }

    friend constexpr bool operator>(const real& lhs, const real& rhs) noexcept {
        return lhs.value > rhs.value + eps();
    }

    friend constexpr bool operator<=(const real& lhs, const real& rhs) noexcept {
        return lhs.value < rhs.value + eps();
    }

    friend constexpr bool operator>=(const real& lhs, const real& rhs) noexcept {
        return lhs.value > rhs.value - eps();
    }

    friend std::ostream& operator<<(std::ostream& os, const real& r) {
        return os << std::fixed << std::setprecision(precision()) << r.value;
    }

    friend std::istream& operator>>(std::istream& is, real& r) {
        value_type x;
        is >> x;
        r.value = x;
        return is;
    }

private:
    value_type value;
};

} // namespace ds
} // namespace cp
#endif