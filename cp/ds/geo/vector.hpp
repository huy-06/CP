#include <iostream>
#include "../../alg/geo/common.hpp"

#ifndef CP_DS_GEO_VECTOR
#define CP_DS_GEO_VECTOR

namespace cp {
namespace ds {

template <class Tp>
class point;

/**
 * @brief Lớp Vector hình học 2D biểu diễn độ dời, hướng và độ lớn.
 * @tparam Tp Kiểu tọa độ số học (long long, double, ds::real,...).
 */
template <class Tp>
class vector {
public:
    using value_type = Tp;

    Tp x{};
    Tp y{};

    constexpr vector() = default;
    
    constexpr vector(const Tp& x, const Tp& y) : x(x), y(y) {}

    // Ép kiểu tường minh từ điểm (định nghĩa trong point.hpp)
    explicit constexpr vector(const point<Tp>& p);

    template <class U>
    explicit constexpr operator vector<U>() const {
        return vector<U>(static_cast<U>(x), static_cast<U>(y));
    }

    constexpr vector operator+() const {
        return *this;
    }

    constexpr vector operator-() const {
        return vector(-x, -y);
    }

    constexpr vector& operator+=(const vector& v) & {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr vector& operator-=(const vector& v) & {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    constexpr vector& operator*=(const Tp& k) & {
        x *= k;
        y *= k;
        return *this;
    }

    constexpr vector& operator/=(const Tp& k) & {
        x /= k;
        y /= k;
        return *this;
    }

    friend constexpr vector operator+(vector a, const vector& b) {
        return a += b;
    }

    friend constexpr vector operator-(vector a, const vector& b) {
        return a -= b;
    }

    friend constexpr vector operator*(vector a, const Tp& k) {
        return a *= k;
    }

    friend constexpr vector operator*(const Tp& k, vector a) {
        return a *= k;
    }

    friend constexpr vector operator/(vector a, const Tp& k) {
        return a /= k;
    }

    /**
     * @brief Tích vô hướng (dot product) của 2 vector: u . v = u.x * v.x + u.y * v.y
     */
    constexpr Tp dot(const vector& v) const {
        return x * v.x + y * v.y;
    }

    /**
     * @brief Tích có hướng 2D (cross product) của 2 vector: u x v = u.x * v.y - u.y * v.x
     */
    constexpr Tp cross(const vector& v) const {
        return x * v.y - y * v.x;
    }

    /**
     * @brief Bình phương độ dài Euclidean của vector.
     */
    constexpr Tp square() const {
        return dot(*this);
    }

    /**
     * @brief Độ dài Euclidean của vector.
     */
    auto length() const {
        return alg::math::sqrt(square());
    }

    /**
     * @brief Trả về vector đơn vị có độ dài bằng 1.
     */
    auto unit() const {
        return *this / length();
    }

    /**
     * @brief Trả về vector xoay 90 độ ngược chiều kim đồng hồ: (-y, x).
     */
    constexpr vector rotate90() const {
        return vector(-y, x);
    }

    /**
     * @brief Trả về vector xoay 90 độ cùng chiều kim đồng hồ: (y, -x).
     */
    constexpr vector rotate270() const {
        return vector(y, -x);
    }

    /**
     * @brief Xoay vector ngược chiều kim đồng hồ một góc rad (radian).
     * @param rad Góc xoay tính bằng radian.
     */
    template <typename Float>
    auto rotate(const Float& rad) const {
        auto c = alg::math::cos(rad);
        auto s = alg::math::sin(rad);
        return vector<decltype(x * c)>(x * c - y * s, x * s + y * c);
    }

    /**
     * @brief Góc cực của vector trong khoảng [-pi, pi].
     */
    auto angle() const {
        return alg::math::atan2(y, x);
    }

    /**
     * @brief Kiểm tra vector có phải vector không (0, 0) hay không.
     */
    constexpr bool is_zero() const {
        return x == Tp(0) && y == Tp(0);
    }

    friend constexpr bool operator==(const vector& a, const vector& b) {
        return a.x == b.x && a.y == b.y;
    }

    friend constexpr bool operator!=(const vector& a, const vector& b) {
        return !(a == b);
    }

    friend constexpr bool operator<(const vector& a, const vector& b) {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }

    friend constexpr bool operator>(const vector& a, const vector& b) {
        return b < a;
    }

    friend constexpr bool operator<=(const vector& a, const vector& b) {
        return !(b < a);
    }

    friend constexpr bool operator>=(const vector& a, const vector& b) {
        return !(a < b);
    }

    friend std::istream& operator>>(std::istream& is, vector& v) {
        return is >> v.x >> v.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const vector& v) {
        return os << "vec(" << v.x << ", " << v.y << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_VECTOR
