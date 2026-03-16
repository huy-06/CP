#include <cassert>
#include <cmath>
#include <iostream>
#include "../../../alg/math/number_theory/numeric.hpp"

#ifndef CP_DS_FRACTION
#define CP_DS_FRACTION
namespace cp {
namespace ds {

template <typename Tp>
class fraction {
public:
    using value_type = Tp;

    constexpr fraction(const value_type& n = 0, const value_type& d = 1) : x(n), y(d) {
        assert(d != static_cast<value_type>(0));
        norm();
    }

    constexpr fraction(const long double& value) {
        if (value == static_cast<value_type>(0.0)) {
            x = 0;
            y = 1;
            return;
        }
        x = static_cast<value_type>(std::round(value * static_cast<value_type>(1e15)));
        y = static_cast<value_type>(1e15);
        norm();
    }

    // numerator
    constexpr value_type num() const {
        return x;
    }

    // denominator
    constexpr value_type den() const {
        return y;
    }

    constexpr fraction operator-() const {
        return fraction(-x, y);
    }

    constexpr fraction& operator+=(const fraction& rhs) & {
        x = x * rhs.y + rhs.x * y;
        y = y * rhs.y;
        norm();
        return *this;
    }

    constexpr fraction& operator-=(const fraction& rhs) & {
        x = x * rhs.y - rhs.x * y;
        y = y * rhs.y;
        norm();
        return *this;
    }

    constexpr fraction& operator*=(const fraction& rhs) & {
        x = x * rhs.x;
        y = y * rhs.y;
        norm();
        return *this;
    }

    constexpr fraction& operator/=(const fraction& rhs) & {
        assert(rhs.x != 0);
        x = x * rhs.y;
        y = y * rhs.x;
        norm();
        return *this;
    }

    constexpr fraction& operator++() & {
        *this += fraction(1);
        return *this;
    }

    constexpr fraction operator++(int) & {
        fraction temp = *this;
        ++(*this);
        return temp;
    }

    constexpr fraction& operator--() & {
        *this -= fraction(1);
        return *this;
    }

    constexpr fraction operator--(int) & {
        fraction temp = *this;
        --(*this);
        return temp;
    }

    friend constexpr fraction operator+(fraction lhs, const fraction& rhs) {
        lhs += rhs;
        return lhs;
    }

    friend constexpr fraction operator-(fraction lhs, const fraction& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr fraction operator*(fraction lhs, const fraction& rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr fraction operator/(fraction lhs, const fraction& rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend constexpr bool operator==(const fraction& lhs, const fraction& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    friend constexpr bool operator!=(const fraction& lhs, const fraction& rhs) {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const fraction& lhs, const fraction& rhs) {
        return lhs.x * rhs.y < rhs.x * lhs.y;
    }

    friend constexpr bool operator>(const fraction& lhs, const fraction& rhs) {
        return rhs < lhs;
    }

    friend constexpr bool operator<=(const fraction& lhs, const fraction& rhs) {
        return !(rhs < lhs);
    }

    friend bool operator>=(const fraction& lhs, const fraction& rhs) {
        return !(lhs < rhs);
    }

    friend std::istream& operator>>(std::istream& is, fraction& f) {
        std::string s;
        if (is >> s) {
            if (s.find('.') == std::string::npos) {
                f = fraction(value_type(s.c_str()));
            } else {
                f = fraction(std::stod(s));
            }
        }
        return is;
    }

    friend constexpr std::ostream& operator<<(std::ostream& os, const fraction& f) {
        if (f.y == static_cast<value_type>(1)) {
            os << f.x;
        } else {
            os << f.x << '/' << f.y;
        }
        return os;
    }

private:
    value_type x, y;

    constexpr void norm() {
        if (y < 0) {
            x = -x;
            y = -y;
        }
        value_type g = cp::alg::gcd(x, y);
        if (g != 0) {
            x /= g;
            y /= g;
        }
    }
};


} // namespace ds
} // namespace cp
#endif