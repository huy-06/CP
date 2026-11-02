#include <string>
#include <vector>
#include <cassert>
#include "line.hpp"
#include "ray.hpp"

#ifndef CP_DS_GEO_INTERSECTION
#define CP_DS_GEO_INTERSECTION

namespace cp {
namespace ds {

/**
 * @brief Lớp biểu diễn kết quả giao nhau giữa các đối tượng hình học.
 * Kiểu giao có thể là: NONE (0), POINT (1), POINTS (2), SEGMENT (3), RAY (4), LINE (5).
 */
template <class Tp>
class intersection {
public:
    using value_type = Tp;

    enum class kind {
        NONE = 0,     // Không giao nhau
        POINT = 1,    // Giao tại 1 điểm duy nhất
        POINTS = 2,   // Giao tại 2 điểm (ví dụ: đường tròn cắt đường tròn/đường thẳng)
        SEGMENT = 3,  // Giao nhau là một đoạn thẳng
        RAY = 4,      // Giao nhau là một tia
        LINE = 5      // Giao nhau là cả một đường thẳng (trùng nhau)
    };

    kind type{kind::NONE};
    point<value_type> a{};
    point<value_type> b{};

    constexpr intersection() = default;

    constexpr explicit intersection(kind k) : type(k) {}

    constexpr intersection(kind k, const point<value_type>& a, const point<value_type>& b = {})
        : type(k), a(a), b(b) {}

    static constexpr intersection make_none() {
        return intersection(kind::NONE);
    }

    static constexpr intersection make_point(const point<value_type>& p) {
        return intersection(kind::POINT, p);
    }

    static constexpr intersection make_points(const point<value_type>& p1, const point<value_type>& p2) {
        return intersection(kind::POINTS, p1, p2);
    }

    static constexpr intersection make_segment(const point<value_type>& p1, const point<value_type>& p2) {
        return intersection(kind::SEGMENT, p1, p2);
    }

    static constexpr intersection make_ray(const point<value_type>& start, const vector<value_type>& dir) {
        return intersection(kind::RAY, start, start + dir);
    }

    static constexpr intersection make_line(const point<value_type>& p, const vector<value_type>& dir) {
        return intersection(kind::LINE, p, p + dir);
    }

    constexpr explicit operator bool() const {
        return type != kind::NONE;
    }

    constexpr bool is_none() const {
        return type == kind::NONE;
    }

    constexpr bool is_point() const {
        return type == kind::POINT;
    }

    constexpr bool is_points() const {
        return type == kind::POINTS;
    }

    constexpr bool is_segment() const {
        return type == kind::SEGMENT;
    }

    constexpr bool is_ray() const {
        return type == kind::RAY;
    }

    constexpr bool is_line() const {
        return type == kind::LINE;
    }

    std::string get_type() const {
        switch (type) {
            case kind::NONE: return "none";
            case kind::POINT: return "point";
            case kind::POINTS: return "points";
            case kind::SEGMENT: return "segment";
            case kind::RAY: return "ray";
            case kind::LINE: return "line";
        }
        return "unknown";
    }

    point<value_type> get_point() const {
        assert(is_point());
        return a;
    }

    std::vector<point<value_type>> get_points() const {
        if (is_point()) return { a };
        if (is_points()) return { a, b };
        return {};
    }

    segment<value_type> get_segment() const {
        assert(is_segment());
        return segment<value_type>(a, b);
    }

    ray<value_type> get_ray() const {
        assert(is_ray());
        return ray<value_type>(a, b - a);
    }

    line<value_type> get_line() const {
        assert(is_line());
        return line<value_type>(a, b - a);
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GEO_INTERSECTION
