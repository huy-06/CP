#include "point.hpp"

#ifndef CP_DS_GEO_RAY
#define CP_DS_GEO_RAY

namespace cp {
namespace ds {

/**
 * @brief Lớp Tia 2D xuất phát từ điểm gốc `p` và bắn vô tận theo hướng vector `v`.
 * @tparam Tp Kiểu tọa độ số học.
 */
template <class Tp>
class ray {
public:
    using value_type = Tp;

    point<value_type> p{};
    vector<value_type> v{};

    constexpr ray() = default;

    constexpr ray(const point<value_type>& p, const vector<value_type>& v) : p(p), v(v) {}

    constexpr ray(const point<value_type>& a, const point<value_type>& b) : p(a), v(b - a) {}

    friend constexpr bool operator==(const ray& r1, const ray& r2) {
        return r1.p == r2.p && r1.v == r2.v;
    }

    friend constexpr bool operator!=(const ray& r1, const ray& r2) {
        return !(r1 == r2);
    }

    friend std::ostream& operator<<(std::ostream& os, const ray& r) {
        return os << "ray(" << r.p << " -> " << r.v << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_RAY
