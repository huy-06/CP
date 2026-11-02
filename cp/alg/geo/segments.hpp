#include "lines.hpp"

#ifndef CP_ALG_GEO_SEGMENTS
#define CP_ALG_GEO_SEGMENTS

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Chiều dài của đoạn thẳng s.
 */
template <class Tp>
auto length(const ds::segment<Tp>& s) {
    return s.length();
}

/**
 * @brief Kiểm tra điểm `p` có nằm trên đoạn thẳng `s` không (tính cả 2 đầu mút).
 */
template <class Tp>
constexpr bool is_on(const ds::point<Tp>& p, const ds::segment<Tp>& s) {
    if (orient(s.a, s.b, p) != 0) return false;
    return dot(p - s.a, p - s.b) <= Tp(0);
}

/**
 * @brief Kiểm tra hai hộp bao (Bounding Box) của hai đoạn thẳng s1 và s2 có giao nhau không.
 */
template <class Tp>
constexpr bool bounding_box_overlap(const ds::segment<Tp>& s1, const ds::segment<Tp>& s2) {
    auto [min_x1, max_x1] = std::minmax(s1.a.x, s1.b.x);
    auto [min_y1, max_y1] = std::minmax(s1.a.y, s1.b.y);
    auto [min_x2, max_x2] = std::minmax(s2.a.x, s2.b.x);
    auto [min_y2, max_y2] = std::minmax(s2.a.y, s2.b.y);

    return max_x1 >= min_x2 && max_x2 >= min_x1
        && max_y1 >= min_y2 && max_y2 >= min_y1;
}

/**
 * @brief Kiểm tra nhanh hai đoạn thẳng s1 và s2 có cắt nhau không (kể cả chạm đầu mút hay trùng một phần).
 */
template <class Tp>
constexpr bool is_intersect(const ds::segment<Tp>& s1, const ds::segment<Tp>& s2) {
    if (!bounding_box_overlap(s1, s2)) return false;

    int o1 = orient(s1.a, s1.b, s2.a);
    int o2 = orient(s1.a, s1.b, s2.b);
    int o3 = orient(s2.a, s2.b, s1.a);
    int o4 = orient(s2.a, s2.b, s1.b);

    if (o1 * o2 <= 0 && o3 * o4 <= 0) {
        return true;
    }
    return false;
}

/**
 * @brief Hình chiếu của điểm `p` lên đoạn thẳng `s` (giới hạn trong đoạn từ a đến b).
 */
template <class Tp>
auto project(const ds::point<Tp>& p, const ds::segment<Tp>& s) {
    auto v = s.to_vector();
    auto len_sq = v.square();
    if (len_sq == 0) return s.a;

    auto t = dot(p - s.a, v) / len_sq;
    if (t < Tp(0)) t = Tp(0);
    if (t > Tp(1)) t = Tp(1);
    return s.a + v * t;
}

/**
 * @brief Đường trung trực của đoạn thẳng `s`.
 */
template <class Tp>
auto perpendicular_bisector(const ds::segment<Tp>& s) {
    auto mid = s.mid();
    auto norm = s.to_vector().rotate90();
    return ds::line(mid, norm);
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_SEGMENTS
