#include <algorithm>
#include <vector>
#include "../../ds/geo/point.hpp"

#ifndef CP_ALG_GEO_VECTORS
#define CP_ALG_GEO_VECTORS

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Tích vô hướng (dot product) của 2 vector: u . v = u.x * v.x + u.y * v.y
 */
template <class Tp>
constexpr Tp dot(const ds::vector<Tp>& u, const ds::vector<Tp>& v) {
    return u.dot(v);
}

/**
 * @brief Tích có hướng 2D (cross product) của 2 vector: u x v = u.x * v.y - u.y * v.x
 */
template <class Tp>
constexpr Tp cross(const ds::vector<Tp>& u, const ds::vector<Tp>& v) {
    return u.cross(v);
}

/**
 * @brief Tích có hướng của 2 vector (b - a) và (c - a).
 * Dương nếu c nằm bên trái đoạn có hướng a -> b (ngược chiều kim đồng hồ - CCW).
 * Âm nếu c nằm bên phải đoạn có hướng a -> b (cùng chiều kim đồng hồ - CW).
 * Bằng 0 nếu 3 điểm a, b, c thẳng hàng.
 */
template <class Tp>
constexpr Tp cross(const ds::point<Tp>& a, const ds::point<Tp>& b, const ds::point<Tp>& c) {
    return cross(b - a, c - a);
}

/**
 * @brief Kiểm tra hướng rẽ (Orientation / CCW test) của bộ 3 điểm (a, b, c).
 * @return 1 nếu rẽ trái (CCW), -1 nếu rẽ phải (CW), 0 nếu thẳng hàng.
 */
template <class Tp>
constexpr int orient(const ds::point<Tp>& a, const ds::point<Tp>& b, const ds::point<Tp>& c) {
    return sgn(cross(a, b, c));
}

/**
 * @brief Bí danh cho hàm orient (kiểm tra ngược chiều kim đồng hồ).
 */
template <class Tp>
constexpr int ccw(const ds::point<Tp>& a, const ds::point<Tp>& b, const ds::point<Tp>& c) {
    return orient(a, b, c);
}

/**
 * @brief Bình phương độ dài của vector v.
 */
template <class Tp>
constexpr Tp square(const ds::vector<Tp>& v) {
    return v.square();
}

/**
 * @brief Độ dài Euclidean của vector v.
 */
template <class Tp>
auto length(const ds::vector<Tp>& v) {
    return v.length();
}

/**
 * @brief Chuẩn hóa vector v về vector đơn vị có độ dài bằng 1.
 */
template <class Tp>
auto normalize(const ds::vector<Tp>& v) {
    return v.unit();
}

/**
 * @brief Vector pháp tuyến của v (xoay 90 độ CCW: (-y, x)).
 */
template <class Tp>
constexpr ds::vector<Tp> normal(const ds::vector<Tp>& v) {
    return v.rotate90();
}

/**
 * @brief Vector vuông góc với v (xoay 90 độ CCW).
 */
template <class Tp>
constexpr ds::vector<Tp> perp(const ds::vector<Tp>& v) {
    return v.rotate90();
}

/**
 * @brief Xoay vector v một góc rad (radian) ngược chiều kim đồng hồ.
 */
template <class Tp, typename Float>
auto rotate(const ds::vector<Tp>& v, const Float& rad) {
    return v.rotate(rad);
}

/**
 * @brief Góc cực của vector v trong khoảng [-pi, pi].
 */
template <class Tp>
auto angle(const ds::vector<Tp>& v) {
    return v.angle();
}

/**
 * @brief Góc giữa 2 vector u và v trong khoảng [0, pi].
 */
template <class Tp>
auto angle(const ds::vector<Tp>& u, const ds::vector<Tp>& v) {
    auto cos_val = dot(u, v) / (length(u) * length(v));
    if (cos_val > Tp(1)) cos_val = Tp(1);
    if (cos_val < -Tp(1)) cos_val = -Tp(1);
    return math::acos(cos_val);
}

/**
 * @brief Hình chiếu vector của `u` lên vector `onto`.
 */
template <class Tp>
auto project(const ds::vector<Tp>& u, const ds::vector<Tp>& onto) {
    return onto * (dot(u, onto) / onto.square());
}

/**
 * @brief Vector đối xứng/phản xạ của `u` qua phương vector `axis`.
 */
template <class Tp>
auto reflect(const ds::vector<Tp>& u, const ds::vector<Tp>& axis) {
    return project(u, axis) * Tp(2) - u;
}

/**
 * @brief Phân loại nửa mặt phẳng trên/dưới cho việc sắp xếp góc cực số nguyên:
 * 1 nếu y > 0 hoặc (y == 0 và x >= 0) (nửa mặt phẳng trên, góc trong [0, pi))
 * 0 nếu y < 0 hoặc (y == 0 và x < 0)  (nửa mặt phẳng dưới, góc trong [-pi, 0))
 */
template <class Tp>
constexpr int polar_half(const ds::vector<Tp>& v) {
    return (v.y > Tp(0) || (v.y == Tp(0) && v.x >= Tp(0))) ? 1 : 0;
}

/**
 * @brief So sánh góc cực giữa 2 vector: sắp xếp ngược chiều kim đồng hồ bắt đầu từ trục hoành dương (Ox).
 * Chính xác tuyệt đối trên tọa độ số nguyên (không qua hàm lượng giác, không mất mát độ chính xác).
 */
template <class Tp>
constexpr bool polar_less(const ds::vector<Tp>& a, const ds::vector<Tp>& b) {
    int ha = polar_half(a);
    int hb = polar_half(b);
    if (ha != hb) return ha > hb;
    auto cp = cross(a, b);
    if (cp != 0) return cp > 0;
    return a.square() < b.square();
}

/**
 * @brief Sắp xếp danh sách các điểm theo góc cực ngược chiều kim đồng hồ quanh điểm gốc `origin`.
 */
template <class Tp>
void polar_sort(std::vector<ds::point<Tp>>& pts, const ds::point<Tp>& origin = ds::point<Tp>{}) {
    std::sort(pts.begin(), pts.end(), [&](const ds::point<Tp>& a, const ds::point<Tp>& b) {
        return polar_less(a - origin, b - origin);
    });
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_VECTORS
