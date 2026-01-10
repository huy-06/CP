#include <vector>
#include "lines.hpp"
#include "../../ds/geometry/intersection.hpp"

#ifndef CP_ALG_GEOMETRY_INTERSECT
#define CP_ALG_GEOMETRY_INTERSECT
namespace cp {
namespace alg {

template <class Tp>
ds::intersection<Tp> intersect(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    Tp det = cross(l1.v, l2.v);

    if (det == 0) {
        if (is_on(l2.p, l1)) 
            return ds::intersection<Tp>(3, l1.p, l1.p + l1.v);
        return ds::intersection<Tp>();
    }

    Tp t = cross(l2.p - l1.p, l2.v) / det;
    return  ds::intersection<Tp>(1, l1.p + l1.v * t);
}

template <class Tp>
ds::intersection<Tp> intersect(const ds::segment<Tp>& s1, const ds::segment<Tp>& s2) {
    auto d1 = cross(s1.b - s1.a, s2.a - s1.a);
    auto d2 = cross(s1.b - s1.a, s2.b - s1.a);
    auto d3 = cross(s2.b - s2.a, s1.a - s2.a);
    auto d4 = cross(s2.b - s2.a, s1.b - s2.a);

    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) 
     && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        ds::line<Tp> l1(s1), l2(s2);
        return intersect(l1, l2); 
    }

    if (d1 == 0 && d2 == 0 && d3 == 0 && d4 == 0) {
        auto p1 = s1.a, p2 = s1.b;
        auto p3 = s2.a, p4 = s2.b;
        
        if (p1 > p2) std::swap(p1, p2);
        if (p3 > p4) std::swap(p3, p4);

        auto l = std::max(p1, p3);
        auto r = std::min(p2, p4);

        if (l < r) return ds::intersection<Tp>(2, l, r);
        if (l == r) return ds::intersection<Tp>(1, l);
        return ds::intersection<Tp>();
    }

    if (is_on(s2.a, s1)) return ds::intersection<Tp>(1, s2.a);
    if (is_on(s2.b, s1)) return ds::intersection<Tp>(1, s2.b);
    if (is_on(s1.a, s2)) return ds::intersection<Tp>(1, s1.a);
    if (is_on(s1.b, s2)) return ds::intersection<Tp>(1, s1.b);

    return ds::intersection<Tp>();
}

template <class Tp>
ds::intersection<Tp> intersect(const ds::segment<Tp>& s, const ds::line<Tp>& l) {
    auto cp1 = cross(l.v, s.a - l.p);
    auto cp2 = cross(l.v, s.b - l.p);

    if (cp1 == 0 && cp2 == 0) {
        return ds::intersection<Tp>(2, s.a, s.b);
    }

    if ((cp1 > 0 && cp2 < 0) || (cp1 < 0 && cp2 > 0)) {
        ds::line<Tp> ln(s); 
        return intersect(l, ln);
    }

    if (cp1 == 0) return ds::intersection<Tp>(1, s.a);
    if (cp2 == 0) return ds::intersection<Tp>(1, s.b);

    return ds::intersection<Tp>();
}

template <class Tp>
ds::intersection<Tp> intersect(const ds::line<Tp>& l, const ds::segment<Tp>& s) {
    return intersect(s, l);
}

} // namespace alg
} // namespace cp
#endif