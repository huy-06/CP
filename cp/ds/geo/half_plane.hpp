#include "line.hpp"

#ifndef CP_DS_GEO_HALF_PLANE
#define CP_DS_GEO_HALF_PLANE

namespace cp {
namespace ds {

/**
 * @brief Lớp Nửa mặt phẳng 2D có hướng.
 * Biểu diễn tập hợp tất cả các điểm `q` nằm bên trái đường thẳng định hướng (p -> p + v):
 * v.cross(q - p) >= 0
 * @tparam Tp Kiểu tọa độ số học.
 */
template <class Tp>
class half_plane {
public:
    using value_type = Tp;

    point<value_type> p{};
    vector<value_type> v{};
    decltype(alg::math::atan2(Tp(), Tp())) angle{};

    constexpr half_plane() = default;
    
    constexpr half_plane(const point<value_type>& p, const vector<value_type>& v)
        : p(p), v(v), angle(alg::math::atan2(v.y, v.x)) {}

    constexpr half_plane(const point<value_type>& a, const point<value_type>& b)
        : half_plane(a, b - a) {}

    constexpr half_plane(const line<value_type>& l)
        : half_plane(l.p, l.v) {}

    /**
     * @brief Khởi tạo nửa mặt phẳng từ bất phương trình tổng quát: a*x + b*y + c >= 0.
     */
    constexpr half_plane(const value_type& a, const value_type& b, const value_type& c) {
        v = vector<value_type>(-b, a);
        if (alg::math::abs(a) > alg::math::abs(b)) {
            p = point<value_type>(-c / a, value_type(0));
        } else {
            p = point<value_type>(value_type(0), -c / b);
        }
        angle = alg::math::atan2(v.y, v.x);
    }

    /**
     * @brief Xác định phía của điểm `q` đối với đường biên của nửa mặt phẳng.
     * @return > 0 nếu q nằm hẳn bên trái, 0 nếu nằm trên đường biên, < 0 nếu nằm bên phải.
     */
    constexpr int side(const point<value_type>& q) const {
        return alg::geo::sgn(v.cross(q - p));
    }

    /**
     * @brief Kiểm tra điểm q có thuộc nửa mặt phẳng không (tính cả bờ).
     */
    constexpr bool contains(const point<value_type>& q) const {
        return side(q) >= 0;
    }

    /**
     * @brief Kiểm tra điểm q có nằm hẳn bên trong nửa mặt phẳng không (không tính bờ).
     */
    constexpr bool strictly_contains(const point<value_type>& q) const {
        return side(q) > 0;
    }

    /**
     * @brief So sánh sắp xếp theo góc cực của vector chỉ phương v.
     * Nếu cùng góc cực, chọn nửa mặt phẳng nằm xa hơn về bên trái (chặt chẽ hơn).
     */
    friend bool operator<(const half_plane& a, const half_plane& b) {
        if (a.angle != b.angle) {
            return a.angle < b.angle;
        }
        return a.side(b.p) < 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const half_plane& hp) {
        return os << "half_plane(" << hp.p << " + t * " << hp.v << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_HALF_PLANE
