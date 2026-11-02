#pragma once
#include <utility>
#include "vectors.hpp"
#include "../../ds/geo/line.hpp"

#ifndef CP_ALG_GEO_LINES
#define CP_ALG_GEO_LINES

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Kiểm tra điểm `p` có nằm trên đường thẳng `l` không.
 */
template <class Tp>
constexpr bool is_on(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return cross(l.v, p - l.p) == 0;
}

/**
 * @brief Xác định phía của điểm `p` đối với đường thẳng định hướng `l`.
 * @return 1 nếu nằm bên trái, 0 nếu nằm trên đường thẳng, -1 nếu nằm bên phải.
 */
template <class Tp>
constexpr int side(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return sgn(cross(l.v, p - l.p));
}

/**
 * @brief Kiểm tra điểm `p` có nằm hẳn bên trái đường thẳng định hướng `l` không.
 */
template <class Tp>
constexpr bool is_left(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return side(p, l) > 0;
}

/**
 * @brief Kiểm tra điểm `p` có nằm hẳn bên phải đường thẳng định hướng `l` không.
 */
template <class Tp>
constexpr bool is_right(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return side(p, l) < 0;
}

/**
 * @brief Kiểm tra hai đường thẳng l1 và l2 có song song không.
 */
template <class Tp>
constexpr bool is_parallel(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    return cross(l1.v, l2.v) == 0;
}

/**
 * @brief Kiểm tra hai đường thẳng l1 và l2 có vuông góc không.
 */
template <class Tp>
constexpr bool is_orthogonal(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    return dot(l1.v, l2.v) == 0;
}

/**
 * @brief Kiểm tra hai đường thẳng l1 và l2 có trùng nhau không.
 */
template <class Tp>
constexpr bool is_same(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    return is_parallel(l1, l2) && is_on(l2.p, l1);
}

/**
 * @brief Hình chiếu vuông góc của điểm `p` lên đường thẳng `l`.
 */
template <class Tp>
auto project(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    auto t = dot(p - l.p, l.v) / l.v.square();
    return l.p + l.v * t;
}

/**
 * @brief Điểm đối xứng của `p` qua đường thẳng `l`.
 */
template <class Tp>
auto reflect(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    auto proj = project(p, l);
    return proj + (proj - p);
}

/**
 * @brief Tìm hai đường phân giác của góc tạo bởi hai đường thẳng cắt nhau l1 và l2.
 * @return Cặp hai đường phân giác vuông góc với nhau.
 */
template <class Tp>
auto bisector(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    auto u1 = l1.v.unit();
    auto u2 = l2.v.unit();
    auto det = cross(l1.v, l2.v);
    auto t = cross(l2.p - l1.p, l2.v) / det;
    auto inter = l1.p + l1.v * t;

    auto b1 = u1 + u2;
    auto b2 = u1 - u2;
    return std::make_pair(ds::line(inter, b1), ds::line(inter, b2));
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_LINES
