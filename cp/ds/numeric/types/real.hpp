#include <iostream>
#include <cmath>
// #include <iomanip>
#include "../../init/constexpr.hpp"

#ifndef CP_DS_REAL_STATIC_EPS_REAL
#define CP_DS_REAL_STATIC_EPS_REAL
namespace cp {
namespace ds {

#if __cplusplus >= 202002L
template <auto EPS>
class real {
public:
    using value_type = std::decay_t<decltype(EPS)>;

    static_assert(std::is_floating_point_v<value_type>, "EPS must be a floating point number");
#else
template <long double EPS>
class real {
public:
    using value_type = long double;
#endif

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
        return os << r.value;
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

namespace std {

#if __cplusplus >= 202002L
#define REAL_TEMPLATE template <auto EPS>
#else
#define REAL_TEMPLATE template <long double EPS>
#endif

REAL_TEMPLATE
constexpr cp::ds::real<EPS> abs(const cp::ds::real<EPS>& x) noexcept {
    return (x < cp::ds::real<EPS>(0)) ? -x : x;
}

REAL_TEMPLATE
inline cp::ds::real<EPS> sqrt(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::sqrt(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> cbrt(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::cbrt(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> pow(const cp::ds::real<EPS>& base, const cp::ds::real<EPS>& exp) {
    return cp::ds::real<EPS>(std::pow(base.val(), exp.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> pow(const cp::ds::real<EPS>& base, typename cp::ds::real<EPS>::value_type exp) {
    return cp::ds::real<EPS>(std::pow(base.val(), exp));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> log(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::log(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> log10(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::log10(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> exp(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::exp(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> sin(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::sin(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> cos(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::cos(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> tan(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::tan(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> asin(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::asin(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> acos(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::acos(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> atan(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::atan(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> atan2(const cp::ds::real<EPS>& y, const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::atan2(y.val(), x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> floor(const cp::ds::real<EPS>& x) { return cp::ds::real<EPS>(std::floor(x.val())); }

REAL_TEMPLATE
inline cp::ds::real<EPS> ceil(const cp::ds::real<EPS>& x) { 
    return cp::ds::real<EPS>(std::ceil(x.val()));
}

REAL_TEMPLATE
inline cp::ds::real<EPS> round(const cp::ds::real<EPS>& x) {
    return cp::ds::real<EPS>(std::round(x.val()));
}

REAL_TEMPLATE
class numeric_limits<cp::ds::real<EPS>> {
public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool has_infinity = true;

    static constexpr cp::ds::real<EPS> min() noexcept {
        return cp::ds::real<EPS>(std::numeric_limits<typename cp::ds::real<EPS>::value_type>::min());
    }

    static constexpr cp::ds::real<EPS> max() noexcept {
        return cp::ds::real<EPS>(std::numeric_limits<typename cp::ds::real<EPS>::value_type>::max());
    }

    static constexpr cp::ds::real<EPS> epsilon() noexcept {
        return cp::ds::real<EPS>::eps();
    }

    static constexpr cp::ds::real<EPS> infinity() noexcept {
        return cp::ds::real<EPS>(std::numeric_limits<typename cp::ds::real<EPS>::value_type>::infinity());
    }
};

#undef REAL_TEMPLATE

} // namespace std
#endif