#include "point.hpp"

#ifndef CP_DS_GEO_SEGMENT
#define CP_DS_GEO_SEGMENT

namespace cp {
namespace ds {

/**
 * @brief Lớp Đoạn thẳng 2D nối giữa hai điểm đầu mút `a` và `b`.
 * @tparam Tp Kiểu tọa độ số học.
 */
template <class Tp>
class segment {
public:
    using value_type = Tp;

    point<value_type> a{};
    point<value_type> b{};

    constexpr segment() = default;
    
    constexpr segment(const point<value_type>& a, const point<value_type>& b) : a(a), b(b) {}

    /**
     * @brief Vector chỉ phương nối từ a đến b (b - a).
     */
    constexpr vector<value_type> to_vector() const {
        return b - a;
    }

    /**
     * @brief Tọa độ trung điểm của đoạn thẳng.
     */
    constexpr point<value_type> mid() const {
        return point<value_type>((a.x + b.x) / value_type(2), (a.y + b.y) / value_type(2));
    }

    /**
     * @brief Chiều dài của đoạn thẳng.
     */
    auto length() const {
        return to_vector().length();
    }

    /**
     * @brief Bình phương chiều dài của đoạn thẳng.
     */
    constexpr value_type square_length() const {
        return to_vector().square();
    }

    friend constexpr bool operator==(const segment& s1, const segment& s2) {
        return s1.a == s2.a && s1.b == s2.b;
    }

    friend constexpr bool operator!=(const segment& s1, const segment& s2) {
        return !(s1 == s2);
    }

    friend std::istream& operator>>(std::istream& is, segment& s) {
        return is >> s.a >> s.b;
    }

    friend std::ostream& operator<<(std::ostream& os, const segment& s) {
        return os << "[" << s.a << ", " << s.b << "]";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_SEGMENT
