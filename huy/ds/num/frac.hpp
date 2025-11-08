#include <cassert>
#include <cmath>
#include <iostream>

#ifndef HUY_DS_FRAC
#define HUY_DS_FRAC
namespace huy {
namespace ds {

template <typename Tp = int64_t>
class frac {
public:
    using value_type = Tp;

    template<class T1, class T2>
    constexpr frac(const T1& n = 0, const T2& d = 1) : x(n), y(d) {
        assert(d != 0);
        norm();
    }

    constexpr frac(const long double& value) {
        if (value == static_cast<value_type>(0.0)) {
            x = 0;
            y = 1;
            return;
        }
        x = static_cast<value_type>(std::round(value * static_cast<value_type>(1e15)));
        y = static_cast<value_type>(1e15);
        norm();
    }

    constexpr frac num() const {
        return x;
    }

    constexpr frac den() const {
        return y;
    }

    constexpr frac operator-() const {
        return frac(-x, y);
    }

    constexpr frac& operator+=(const frac& rhs) & {
        x = x * rhs.y + rhs.x * y;
        y = y * rhs.y;
        norm();
        return *this;
    }

    constexpr frac& operator-=(const frac& rhs) & {
        x = x * rhs.y - rhs.x * y;
        y = y * rhs.y;
        norm();
        return *this;
    }

    constexpr frac& operator*=(const frac& rhs) & {
        x = x * rhs.x;
        y = y * rhs.y;
        norm();
        return *this;
    }

    constexpr frac& operator/=(const frac& rhs) & {
        assert(rhs.x != 0);
        x = x * rhs.y;
        y = y * rhs.x;
        norm();
        return *this;
    }

    constexpr frac& operator++() & {
        *this += frac(1);
        return *this;
    }

    constexpr frac operator++(int) & {
        frac temp = *this;
        ++(*this);
        return temp;
    }

    constexpr frac& operator--() & {
        *this -= frac(1);
        return *this;
    }

    constexpr frac operator--(int) & {
        frac temp = *this;
        --(*this);
        return temp;
    }

    friend constexpr frac operator+(frac lhs, const frac& rhs) {
        lhs += rhs;
        return lhs;
    }

    friend constexpr frac operator-(frac lhs, const frac& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr frac operator*(frac lhs, const frac& rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr frac operator/(frac lhs, const frac& rhs) {
        lhs /= rhs;
        return lhs;
    }


    friend constexpr bool operator==(const frac& lhs, const frac& rhs) {
        return lhs.x * rhs.y == rhs.x * lhs.y;
    }

    friend constexpr bool operator!=(const frac& lhs, const frac& rhs) {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(const frac& lhs, const frac& rhs) {
        return lhs.x * rhs.y < rhs.x * lhs.y;
    }

    friend constexpr bool operator>(const frac& lhs, const frac& rhs) {
        return rhs < lhs;
    }

    friend constexpr bool operator<=(const frac& lhs, const frac& rhs) {
        return !(rhs < lhs);
    }

    friend constexpr bool operator>=(const frac& lhs, const frac& rhs) {
        return !(lhs < rhs);
    }

    friend constexpr std::istream& operator>>(std::istream& is, frac& f) {
        std::string s;
        is >> s;
        if (s.find('.') == s.npos) {
            f = frac(std::stoll(s));
        } else {
            f = frac(std::stod(s));
        }
        return is;
    }

    friend constexpr std::ostream& operator<<(std::ostream& os, const frac& f) {
        if (f.y == static_cast<value_type>(1)) {
            os << f.x;
        } else {
            os << static_cast<value_type>(f.x) / f.y;
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
        value_type g = std::__gcd(x, y);
        if (g != 0) {
            x /= g;
            y /= g;
        }
    }
};


} // namespace ds
} // namespace huy
#endif