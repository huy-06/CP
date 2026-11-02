#include "vector.hpp"

#ifndef CP_DS_GEO_POINT
#define CP_DS_GEO_POINT

namespace cp {
namespace ds {

/**
 * @brief Lớp Điểm trong không gian Affine 2D biểu diễn vị trí trên mặt phẳng.
 * @tparam Tp Kiểu tọa độ số học (long long, double, ds::real,...).
 */
template <class Tp>
class point {
public:
    using value_type = Tp;

    Tp x{};
    Tp y{};

    constexpr point() = default;
    
    constexpr point(const Tp& x, const Tp& y) : x(x), y(y) {}

    explicit constexpr point(const vector<Tp>& v) : x(v.x), y(v.y) {}

    template <class U>
    explicit constexpr operator point<U>() const {
        return point<U>(static_cast<U>(x), static_cast<U>(y));
    }

    /**
     * @brief Chuyển điểm thành vector vị trí nối từ gốc tọa độ O(0,0) đến điểm này.
     */
    constexpr vector<Tp> to_vector() const {
        return vector<Tp>(x, y);
    }

    // Phép toán Affine: Điểm + Vector = Điểm, Điểm - Vector = Điểm
    constexpr point& operator+=(const vector<Tp>& v) & {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr point& operator-=(const vector<Tp>& v) & {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    friend constexpr point operator+(point p, const vector<Tp>& v) {
        return p += v;
    }

    friend constexpr point operator+(const vector<Tp>& v, point p) {
        return p += v;
    }

    friend constexpr point operator-(point p, const vector<Tp>& v) {
        return p -= v;
    }

    // Phép toán Affine: Điểm - Điểm = Vector (độ dời từ b đến a)
    friend constexpr vector<Tp> operator-(const point& a, const point& b) {
        return vector<Tp>(a.x - b.x, a.y - b.y);
    }

    friend constexpr bool operator==(const point& a, const point& b) {
        return a.x == b.x && a.y == b.y;
    }

    friend constexpr bool operator!=(const point& a, const point& b) {
        return !(a == b);
    }

    friend constexpr bool operator<(const point& a, const point& b) {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }

    friend constexpr bool operator>(const point& a, const point& b) {
        return b < a;
    }

    friend constexpr bool operator<=(const point& a, const point& b) {
        return !(b < a);
    }

    friend constexpr bool operator>=(const point& a, const point& b) {
        return !(a < b);
    }

    friend std::istream& operator>>(std::istream& is, point& p) {
        return is >> p.x >> p.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const point& p) {
        return os << "point(" << p.x << ", " << p.y << ")";
    }
};

// Khởi tạo vector từ point
template <class Tp>
constexpr vector<Tp>::vector(const point<Tp>& p) : x(p.x), y(p.y) {}

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_POINT
