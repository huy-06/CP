#include "intersect.hpp"

#ifndef CP_ALG_GEO_DISTANCE
#define CP_ALG_GEO_DISTANCE

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Khoảng cách Manhattan (chuẩn L1) giữa hai điểm: |x1 - x2| + |y1 - y2|.
 */
template <class Tp>
constexpr Tp manhattan(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return math::abs(a.x - b.x) + math::abs(a.y - b.y);
}

/**
 * @brief Khoảng cách Euclidean (đường chim bay) giữa hai điểm: |a - b|.
 */
template <class Tp>
auto distance(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return (a - b).length();
}

/**
 * @brief Khoảng cách từ điểm `p` đến đường thẳng vô hạn `l`.
 */
template <class Tp>
auto distance(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return math::abs(cross(l.v, p - l.p)) / l.v.length();
}

template <class Tp>
auto distance(const ds::line<Tp>& l, const ds::point<Tp>& p) {
    return distance(p, l);
}

/**
 * @brief Khoảng cách từ điểm `p` đến đoạn thẳng `s`.
 */
template <class Tp>
auto distance(const ds::point<Tp>& p, const ds::segment<Tp>& s) {
    auto v = s.to_vector();
    auto u = p - s.a;
    auto len_sq = v.square();
    if (len_sq == 0) return distance(p, s.a);

    auto t = dot(u, v) / len_sq;
    if (t <= Tp(0)) return distance(p, s.a);
    if (t >= Tp(1)) return distance(p, s.b);
    return math::abs(cross(v, u)) / v.length();
}

template <class Tp>
auto distance(const ds::segment<Tp>& s, const ds::point<Tp>& p) {
    return distance(p, s);
}

/**
 * @brief Khoảng cách từ điểm `p` đến tia `r`.
 */
template <class Tp>
auto distance(const ds::point<Tp>& p, const ds::ray<Tp>& r) {
    auto u = p - r.p;
    auto len_sq = r.v.square();
    if (len_sq == 0) return distance(p, r.p);

    auto t = dot(u, r.v) / len_sq;
    if (t <= Tp(0)) return distance(p, r.p);
    return math::abs(cross(r.v, u)) / r.v.length();
}

template <class Tp>
auto distance(const ds::ray<Tp>& r, const ds::point<Tp>& p) {
    return distance(p, r);
}

/**
 * @brief Khoảng cách ngắn nhất giữa hai đoạn thẳng s1 và s2.
 */
template <class Tp>
auto distance(const ds::segment<Tp>& s1, const ds::segment<Tp>& s2) {
    if (is_intersect(s1, s2)) return decltype(distance(s1.a, s2))(0);
    return std::min({
        distance(s1.a, s2),
        distance(s1.b, s2),
        distance(s2.a, s1),
        distance(s2.b, s1)
    });
}

/**
 * @brief Khoảng cách ngắn nhất giữa đoạn thẳng s và đường thẳng vô hạn l.
 */
template <class Tp>
auto distance(const ds::segment<Tp>& s, const ds::line<Tp>& l) {
    if (intersect(l, s)) return decltype(distance(s.a, l))(0);
    return std::min(distance(s.a, l), distance(s.b, l));
}

template <class Tp>
auto distance(const ds::line<Tp>& l, const ds::segment<Tp>& s) {
    return distance(s, l);
}

/**
 * @brief Khoảng cách ngắn nhất giữa hai đường thẳng l1 và l2 (bằng 0 nếu cắt nhau, khoảng cách song song nếu không).
 */
template <class Tp>
auto distance(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    if (is_parallel(l1, l2)) return distance(l1.p, l2);
    return decltype(distance(l1.p, l2))(0);
}

/**
 * @brief Khoảng cách ngắn nhất giữa tia r và đường thẳng l.
 */
template <class Tp>
auto distance(const ds::ray<Tp>& r, const ds::line<Tp>& l) {
    if (intersect(r, l)) return decltype(distance(r.p, l))(0);
    return distance(r.p, l);
}

template <class Tp>
auto distance(const ds::line<Tp>& l, const ds::ray<Tp>& r) {
    return distance(r, l);
}

/**
 * @brief Khoảng cách ngắn nhất giữa tia r và đoạn thẳng s.
 */
template <class Tp>
auto distance(const ds::ray<Tp>& r, const ds::segment<Tp>& s) {
    if (intersect(r, s)) return decltype(distance(r.p, s))(0);
    return std::min({
        distance(r.p, s),
        distance(s.a, r),
        distance(s.b, r)
    });
}

template <class Tp>
auto distance(const ds::segment<Tp>& s, const ds::ray<Tp>& r) {
    return distance(r, s);
}

/**
 * @brief Khoảng cách ngắn nhất giữa hai tia r1 và r2.
 */
template <class Tp>
auto distance(const ds::ray<Tp>& r1, const ds::ray<Tp>& r2) {
    if (intersect(r1, r2)) return decltype(distance(r1.p, r2))(0);
    return std::min(distance(r1.p, r2), distance(r2.p, r1));
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_DISTANCE
