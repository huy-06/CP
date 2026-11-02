#include "point.hpp"

#ifndef CP_DS_GEO_CIRCLE
#define CP_DS_GEO_CIRCLE

namespace cp {
namespace ds {

/**
 * @brief Lớp Đường tròn 2D xác định bởi tâm `c` và bán kính `r`.
 * @tparam Tp Kiểu tọa độ và bán kính số học.
 */
template <class Tp>
class circle {
public:
    using value_type = Tp;

    point<value_type> c{};
    value_type r{};

    constexpr circle() = default;

    constexpr circle(const point<value_type>& c, const value_type& r) : c(c), r(r) {}
    
    constexpr circle(const value_type& x, const value_type& y, const value_type& r)
        : c(point<value_type>(x, y)), r(r) {}

    /**
     * @brief Diện tích hình tròn: pi * r^2
     */
    auto area() const {
        return static_cast<value_type>(cst::pi) * r * r;
    }

    /**
     * @brief Chu vi đường tròn: 2 * pi * r
     */
    auto perimeter() const {
        return value_type(2) * static_cast<value_type>(cst::pi) * r;
    }

    /**
     * @brief Xác định vị trí tương đối của điểm p so với đường tròn.
     * @return 1 nếu nằm hẳn bên trong, 0 nếu nằm trên đường tròn, -1 nếu nằm bên ngoài.
     */
    constexpr int locate(const point<value_type>& p) const {
        auto d2 = (p - c).square();
        auto r2 = r * r;
        if (d2 < r2) return 1;
        if (d2 > r2) return -1;
        return 0;
    }

    /**
     * @brief Kiểm tra điểm p có thuộc hình tròn không (tính cả biên).
     */
    constexpr bool contains(const point<value_type>& p) const {
        return locate(p) >= 0;
    }

    /**
     * @brief Kiểm tra điểm p có nằm hoàn toàn bên trong hình tròn không (không tính biên).
     */
    constexpr bool strictly_contains(const point<value_type>& p) const {
        return locate(p) > 0;
    }

    /**
     * @brief Lấy tọa độ một điểm trên đường tròn tại góc cực rad.
     */
    template <typename Float>
    point<Float> eval(const Float& rad) const {
        return point<Float>(c.x + r * alg::math::cos(rad), c.y + r * alg::math::sin(rad));
    }

    friend constexpr bool operator==(const circle& c1, const circle& c2) {
        return c1.c == c2.c && c1.r == c2.r;
    }

    friend constexpr bool operator!=(const circle& c1, const circle& c2) {
        return !(c1 == c2);
    }

    friend std::istream& operator>>(std::istream& is, circle& cir) {
        return is >> cir.c >> cir.r;
    }

    friend std::ostream& operator<<(std::ostream& os, const circle& cir) {
        return os << "circle(" << cir.c << ", r=" << cir.r << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_CIRCLE
