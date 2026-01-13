#include <cmath>
#include <type_traits>
#include "../../../init/constexpr.hpp"

#ifndef CP_ALG_ARITHMETIC
#define CP_ALG_ARITHMETIC
namespace cp {
namespace alg {

template <typename Tp>
constexpr auto abs(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::abs(x);
}

template <typename Tp>
constexpr auto abs(const Tp& x) -> decltype(x.eps(), Tp()) {
    return (x < Tp(0)) ? -x : x;
}

template <typename Tp>
constexpr auto abs(const Tp& x) -> decltype(x.mod(), Tp()) {
    return x;
}

template <typename T1, typename T2>
constexpr auto pow(const T1& base, const T2& exp) -> decltype(base.pow(exp), T1()) {
    return base.pow(exp);
}

template <typename T1, typename T2>
constexpr auto pow(T1 base, T2 exp) -> typename std::enable_if<std::is_integral<T2>::value, T1>::type {
    T1 res = T1(1);
    while (exp > 0) {
        if (exp & 1) 
            res *= base;
        base *= base;
        exp >>= 1;
    }
    return res;
}

template <typename T1, typename T2>
constexpr auto pow(const T1& base, const T2& exp) -> typename std::enable_if<std::is_floating_point<T2>::value, T1>::type {
    return std::pow(base, exp);
}

template <typename Tp>
constexpr auto sqrt(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::sqrt(x);
}

template <typename Tp>
constexpr auto sqrt(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::sqrt(x.val()));
}

template <typename Tp>
constexpr auto sqrt(const Tp& x) -> decltype(x.mod(), Tp()) {
    if (x == 0) return Tp(0);
    using value_type = typename Tp::value_type;

    value_type p = Tp::mod();
    if (x.pow((p - 1) / 2) != 1) return -1;
    if (p % 4 == 3) return x.pow((p + 1) / 4);

    value_type s = 0;
    value_type q = p - 1;
    while ((q & 1) == 0) {
        q >>= 1;
        ++s;
    }

    Tp z = 2;
    while (z.pow(p - 1) / 2== 1) {
        ++z;
    }

    Tp c = z.pow(q);
    Tp r = x.pow((q + 1) / 2);
    Tp t = x.pow(q);
    value_type m = s;

    while (t != 1) {
        int i = 1;
        Tp tmp = t * t;

        while (tmp != 1) {
            tmp *= tmp;
            ++i;
        }

        Tp b = c;
        for (int j = 0; j < m - i - 1; ++j) {
            b *= b;
        }

        m = i;
        c = b * b;
        t *= c;
        r *= b;
    }

    return r;
}

template <typename Tp>
constexpr auto cbrt(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::cbrt(x);
}

template <typename Tp>
constexpr auto cbrt(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::cbrt(x.val()));
}

template <typename Tp>
constexpr auto log(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::log(x);
}

template <typename Tp>
constexpr auto log(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::log(x.val()));
}

template <typename Tp>
constexpr auto log10(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::log10(x);
}

template <typename Tp>
constexpr auto log10(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::log10(x.val()));
}

template <typename Tp>
constexpr auto exp(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::exp(x);
}

template <typename Tp>
constexpr auto exp(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::exp(x.val()));
}

template <typename Tp>
constexpr auto sin(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::sin(x);
}

template <typename Tp>
constexpr auto sin(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::sin(x.val()));
}

template <typename Tp>
constexpr auto cos(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::cos(x);
}

template <typename Tp>
constexpr auto cos(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::cos(x.val()));
}

template <typename Tp>
constexpr auto tan(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::tan(x);
}

template <typename Tp>
constexpr auto tan(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::tan(x.val()));
}

template <typename Tp>
constexpr auto asin(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::asin(x);
}

template <typename Tp>
constexpr auto asin(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::asin(x.val()));
}

template <typename Tp>
constexpr auto acos(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::acos(x);
}

template <typename Tp>
constexpr auto acos(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::acos(x.val()));
}

template <typename Tp>
constexpr auto atan(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::atan(x);
}

template <typename Tp>
constexpr auto atan(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::atan(x.val()));
}

template <typename Tp>
constexpr auto atan2(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::atan2(x);
}

template <typename Tp>
constexpr auto atan2(const Tp& y, const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::atan2(y.val(), x.val()));
}

template <typename Tp>
constexpr auto floor(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::floor(x);
}

template <typename Tp>
constexpr auto floor(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::floor(x.val()));
}

template <typename Tp>
constexpr auto ceil(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::ceil(x);
}

template <typename Tp>
constexpr auto ceil(const Tp& x) -> decltype(x.eps(), Tp()) { 
    return Tp(std::ceil(x.val()));
}

template <typename Tp>
constexpr auto round(const Tp& x) -> typename std::enable_if<std::is_arithmetic<Tp>::value, Tp>::type {
    return std::round(x);
}

template <typename Tp>
constexpr auto round(const Tp& x) -> decltype(x.eps(), Tp()) {
    return Tp(std::round(x.val()));
}

} // namespace alg
} // namespace cp
#endif