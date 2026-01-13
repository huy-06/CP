#include "intersect.hpp"
#include "../../ds/geometry/ray.hpp"

#ifndef CP_ALG_GEOMETRY_DISTANCE
#define CP_ALG_GEOMETRY_DISTANCE
namespace cp {
namespace alg {
    
template <class Tp>
Tp distance(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return length(ds::vector<Tp>(a - b));
}

template <class Tp>
Tp distance(const ds::point<Tp>& p, const ds::segment<Tp>& s) {
    if (s.a == s.b) return distance(p, s.a);

    ds::vector<Tp> v = s.b - s.a;
    ds::vector<Tp> u = p - s.a;

    auto len_sq = square(v);
    auto t = dot(u, v) / len_sq;

    if (t < 0) return distance(p, s.a);
    if (t > 1) return distance(p, s.b);

    return abs(cross(v, u)) / sqrt(len_sq);
}

template <class Tp>
Tp distance(const ds::segment<Tp>& s, const ds::point<Tp>& p) {
    return distance(p, s);
}

template <class Tp>
Tp distance(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return abs(cross(l.v, ds::vector<Tp>(p - l.p))) / length(l.v);
}

template <class Tp>
Tp distance(const ds::line<Tp>& l, const ds::point<Tp>& p) {
    return distance(p, l);
}

template <class Tp>
Tp distance(const ds::segment<Tp>& s1, const ds::segment<Tp>& s2) {
    if (intersect(s1, s2)) return Tp(0);
    
    return std::min({
        distance(s1.a, s2),
        distance(s1.b, s2),
        distance(s2.a, s1),
        distance(s2.b, s1)
    });
}

template <class Tp>
Tp distance(const ds::segment<Tp>& s, const ds::line<Tp>& l) {
    if (intersect(s, l)) return Tp(0);
    return std::min(distance(s.a, l), distance(s.b, l));
}

template <class Tp>
Tp distance(const ds::line<Tp>& l, const ds::segment<Tp>& s) {
    return distance(s, l);
}

template <class Tp>
Tp distance(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    if (is_parallel(l1, l2)) return distance(l1.p, l2);
    return Tp(0);
}

template <class Tp>
Tp distance(const ds::point<Tp>& p, const ds::ray<Tp>& r) {
    ds::vector<Tp> u = p - r.p;
    
    if (dot(u, r.v) <= 0) {
        return distance(p, r.p);
    }

    return abs(cross(r.v, u)) / length(r.v);
}

template <class Tp>
Tp distance(const ds::ray<Tp>& r, const ds::point<Tp>& p) {
    return distance(p, r);
}

template <class Tp>
Tp distance(const ds::ray<Tp>& r, const ds::line<Tp>& l) {
    if (intersect(r, l)) return Tp(0);
    
    return distance(r.p, l);
}

template <class Tp>
Tp distance(const ds::line<Tp>& l, const ds::ray<Tp>& r) {
    return distance(r, l);
}

template <class Tp>
Tp distance(const ds::ray<Tp>& r, const ds::segment<Tp>& s) {
    if (intersect(r, s)) return Tp(0);

    return std::min({
        distance(r.p, s),
        distance(s.a, r),
        distance(s.b, r)
    });
}

template <class Tp>
Tp distance(const ds::segment<Tp>& s, const ds::ray<Tp>& r) {
    return distance(r, s);
}

template <class Tp>
Tp distance(const ds::ray<Tp>& r1, const ds::ray<Tp>& r2) {
    if (intersect(r1, r2)) return Tp(0);

    return std::min(distance(r1.p, r2), distance(r2.p, r1));
}

} // namespace alg
} // namespace cp
#endif