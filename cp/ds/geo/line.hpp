#include <tuple>
#include "segment.hpp"

#ifndef CP_DS_GEO_LINE
#define CP_DS_GEO_LINE

namespace cp {
namespace ds {

/**
 * @brief Lớp Đường thẳng vô hạn 2D đi qua điểm `p` với vector chỉ phương `v`.
 * @tparam Tp Kiểu tọa độ số học.
 */
template <class Tp>
class line {
public:
    using value_type = Tp;

    point<value_type> p{};
    vector<value_type> v{};

    constexpr line() = default;

    constexpr line(const point<value_type>& p, const vector<value_type>& v) : p(p), v(v) {}

    constexpr line(const point<value_type>& a, const point<value_type>& b) : p(a), v(b - a) {}
    
    constexpr line(const segment<value_type>& s) : p(s.a), v(s.b - s.a) {}

    /**
     * @brief Khởi tạo đường thẳng từ phương trình tổng quát: a*x + b*y + c = 0.
     */
    constexpr line(const value_type& a, const value_type& b, const value_type& c) {
        v = vector<value_type>(b, -a);
        if (alg::math::abs(a) > alg::math::abs(b)) {
            p = point<value_type>(-c / a, value_type(0));
        } else {
            p = point<value_type>(value_type(0), -c / b);
        }
    }

    /**
     * @brief Lấy các hệ số của phương trình tổng quát {A, B, C} sao cho A*x + B*y + C = 0.
     */
    constexpr std::tuple<value_type, value_type, value_type> get_equation() const {
        value_type a = -v.y;
        value_type b = v.x;
        value_type c = -(a * p.x + b * p.y);
        return { a, b, c };
    }

    /**
     * @brief Trả về một vector pháp tuyến của đường thẳng (xoay vector chỉ phương 90 độ CCW).
     */
    constexpr vector<value_type> normal() const {
        return v.rotate90();
    }

    /**
     * @brief Tính tọa độ điểm trên đường thẳng theo tham số t: p + v * t.
     */
    constexpr point<value_type> eval(const value_type& t) const {
        return p + v * t;
    }

    friend std::ostream& operator<<(std::ostream& os, const line& l) {
        return os << "line(" << l.p << " + t * " << l.v << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_LINE
