#include "intersect.hpp" 

#ifndef CP_ALG_GEOMETRY_DISTANCE
#define CP_ALG_GEOMETRY_DISTANCE
namespace cp {
namespace alg {
    
template <class Tp>
Tp distance(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return length(a - b);
}

template <class Tp>
Tp distance(const ds::point<Tp>& p, const ds::segment<Tp>& s) {
    if (s.a == s.b) return distance(p, s.a);

    ds::vector v = s.b - s.a;
    ds::vector u = p - s.a;

    auto len_sq = square(v);
    auto t = dot(u, v) / len_sq;

    if (t < 0) return distance(p, s.a);
    if (t > 1) return distance(p, s.b);

    return std::abs(cross(v, u)) / std::sqrt(len_sq);
}

template <class Tp>
Tp distance(const ds::segment<Tp>& s, const ds::point<Tp>& p) {
    return distance(p, s);
}

template <class Tp>
Tp distance(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return std::abs(cross(l.v, p - l.p)) / length(l.v);
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

} // namespace alg
} // namespace cp
#endif