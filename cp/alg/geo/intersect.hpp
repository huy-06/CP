#include "segments.hpp"
#include "rays.hpp"
#include "../../ds/geo/intersection.hpp"

#ifndef CP_ALG_GEO_INTERSECT
#define CP_ALG_GEO_INTERSECT

namespace cp {
namespace alg {
namespace geo {

/**
 * @brief Tìm giao điểm giữa hai đường thẳng vô hạn l1 và l2.
 * @return ds::intersection chứa kiểu NONE, POINT (cắt nhau tại 1 điểm), hoặc LINE (trùng nhau).
 */
template <class Tp>
auto intersect(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    auto det = cross(l1.v, l2.v);

    if (det == 0) {
        if (is_on(l2.p, l1)) {
            return ds::intersection<Tp>::make_line(l1.p, l1.v);
        }
        return ds::intersection<Tp>::make_none();
    }

    auto t = cross(l2.p - l1.p, l2.v) / det;
    return ds::intersection<Tp>::make_point(l1.p + l1.v * t);
}

/**
 * @brief Tìm giao điểm giữa đường thẳng l và đoạn thẳng s.
 */
template <class Tp>
auto intersect(const ds::line<Tp>& l, const ds::segment<Tp>& s) {
    int s1 = side(s.a, l);
    int s2 = side(s.b, l);

    if (s1 == 0 && s2 == 0) {
        return ds::intersection<Tp>::make_segment(s.a, s.b);
    }
    if ((s1 > 0 && s2 > 0) || (s1 < 0 && s2 < 0)) {
        return ds::intersection<Tp>::make_none();
    }

    auto line_res = intersect(l, ds::line<Tp>(s));
    if (line_res.is_point()) {
        return line_res;
    }
    return ds::intersection<Tp>::make_none();
}

template <class Tp>
auto intersect(const ds::segment<Tp>& s, const ds::line<Tp>& l) {
    return intersect(l, s);
}

/**
 * @brief Tìm giao điểm giữa hai đoạn thẳng s1 và s2.
 * Xử lý đầy đủ các trường hợp: cắt tại 1 điểm, trùng một đoạn thẳng, chạm ở đầu mút.
 */
template <class Tp>
auto intersect(const ds::segment<Tp>& s1, const ds::segment<Tp>& s2) {
    if (!bounding_box_overlap(s1, s2)) {
        return ds::intersection<Tp>::make_none();
    }

    auto v1 = s1.to_vector();
    auto v2 = s2.to_vector();
    auto det = cross(v1, v2);

    if (det == 0) {
        if (orient(s1.a, s1.b, s2.a) != 0) {
            return ds::intersection<Tp>::make_none();
        }

        auto p1 = s1.a, p2 = s1.b;
        auto p3 = s2.a, p4 = s2.b;
        if (p2 < p1) std::swap(p1, p2);
        if (p4 < p3) std::swap(p3, p4);

        auto left = std::max(p1, p3);
        auto right = std::min(p2, p4);

        if (left < right) {
            return ds::intersection<Tp>::make_segment(left, right);
        } else if (left == right) {
            return ds::intersection<Tp>::make_point(left);
        }
        return ds::intersection<Tp>::make_none();
    }

    auto t1 = cross(s2.a - s1.a, v2) / det;
    auto t2 = cross(s2.a - s1.a, v1) / det;

    if (t1 >= Tp(0) && t1 <= Tp(1) && t2 >= Tp(0) && t2 <= Tp(1)) {
        return ds::intersection<Tp>::make_point(s1.a + v1 * t1);
    }

    return ds::intersection<Tp>::make_none();
}

/**
 * @brief Tìm giao điểm giữa tia r và đường thẳng l.
 */
template <class Tp>
auto intersect(const ds::ray<Tp>& r, const ds::line<Tp>& l) {
    auto det = cross(r.v, l.v);

    if (det == 0) {
        if (is_on(r.p, l)) {
            return ds::intersection<Tp>::make_ray(r.p, r.v);
        }
        return ds::intersection<Tp>::make_none();
    }

    auto t = cross(l.p - r.p, l.v) / det;
    if (t >= Tp(0)) {
        return ds::intersection<Tp>::make_point(r.p + r.v * t);
    }
    return ds::intersection<Tp>::make_none();
}

template <class Tp>
auto intersect(const ds::line<Tp>& l, const ds::ray<Tp>& r) {
    return intersect(r, l);
}

/**
 * @brief Tìm giao điểm giữa tia r và đoạn thẳng s.
 */
template <class Tp>
auto intersect(const ds::ray<Tp>& r, const ds::segment<Tp>& s) {
    auto det = cross(r.v, s.to_vector());

    if (det == 0) {
        if (orient(r.p, r.p + r.v, s.a) != 0) return ds::intersection<Tp>::make_none();
        bool a_on = is_on(s.a, r);
        bool b_on = is_on(s.b, r);
        if (a_on && b_on) return ds::intersection<Tp>::make_segment(s.a, s.b);
        if (a_on) return ds::intersection<Tp>::make_segment(r.p, s.a);
        if (b_on) return ds::intersection<Tp>::make_segment(r.p, s.b);
        return ds::intersection<Tp>::make_none();
    }

    auto t_ray = cross(s.a - r.p, s.to_vector()) / det;
    auto t_seg = cross(s.a - r.p, r.v) / det;

    if (t_ray >= Tp(0) && t_seg >= Tp(0) && t_seg <= Tp(1)) {
        return ds::intersection<Tp>::make_point(r.p + r.v * t_ray);
    }

    return ds::intersection<Tp>::make_none();
}

template <class Tp>
auto intersect(const ds::segment<Tp>& s, const ds::ray<Tp>& r) {
    return intersect(r, s);
}

/**
 * @brief Tìm giao điểm giữa hai tia r1 và r2.
 */
template <class Tp>
auto intersect(const ds::ray<Tp>& r1, const ds::ray<Tp>& r2) {
    auto det = cross(r1.v, r2.v);

    if (det == 0) {
        if (orient(r1.p, r1.p + r1.v, r2.p) != 0) return ds::intersection<Tp>::make_none();
        if (dot(r1.v, r2.v) > 0) {
            if (is_on(r1.p, r2)) return ds::intersection<Tp>::make_ray(r1.p, r1.v);
            return ds::intersection<Tp>::make_ray(r2.p, r2.v);
        } else {
            if (is_on(r1.p, r2)) return ds::intersection<Tp>::make_segment(r1.p, r2.p);
            return ds::intersection<Tp>::make_none();
        }
    }

    auto t1 = cross(r2.p - r1.p, r2.v) / det;
    auto t2 = cross(r2.p - r1.p, r1.v) / det;

    if (t1 >= Tp(0) && t2 >= Tp(0)) {
        return ds::intersection<Tp>::make_point(r1.p + r1.v * t1);
    }

    return ds::intersection<Tp>::make_none();
}

} // namespace geo
} // namespace alg
} // namespace cp

#endif // CP_ALG_GEO_INTERSECT
