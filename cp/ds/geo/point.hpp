#include <iostream>

#ifndef CP_DS_GEO_POINT
#define CP_DS_GEO_POINT
namespace cp {
namespace ds {

template<class Tp>
struct point {
    Tp x;
    Tp y;

    point(const Tp& x = 0, const Tp& y = 0) : x(x), y(y) {}

    template<class U>
    operator point<U>() {
        return point<U>(U(x), U(y));
    }

    point& operator+=(const point& p) & {
        x += p.x; y += p.y; return *this;
    }

    point& operator-=(const point& p) & {
        x -= p.x; y -= p.y; return *this;
    }

    point& operator*=(const Tp& v) & {
        x *= v; y *= v; return *this;
    }

    point& operator/=(const Tp& v) & {
        x /= v; y /= v; return *this;
    }

    point operator-() const {
        return point(-x, -y);
    }

    friend point operator+(point a, const point& b) {
        return a += b;
    }

    friend point operator-(point a, const point& b) {
        return a -= b;
    }

    friend point operator*(point a, const Tp& b) {
        return a *= b;
    }

    friend point operator/(point a, const Tp& b) {
        return a /= b;
    }

    friend point operator*(const Tp& a, point b) {
        return b *= a;
    }

    friend bool operator==(const point& a, const point& b) {
        return a.x == b.x && a.y == b.y;
    }

    friend bool operator!=(const point& a, const point& b) {
        return !(a == b);
    }

    friend bool operator<(const point& a, const point& b) {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }

    friend bool operator>(const point& a, const point& b) {
        return b < a;
    }

    friend bool operator<=(const point& a, const point& b) {
        return a < b || a == b;
    }

    friend bool operator>=(const point& a, const point& b) {
        return a > b || a == b;
    }

    friend std::istream& operator>>(std::istream& is, point& p) {
        return is >> p.x >> p.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const point& p) {
        return os << "(" << p.x << ", " << p.y << ")";
    }
};

} // namespace ds
} // namespace cp
#endif