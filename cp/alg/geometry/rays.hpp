#include "segments.hpp"
#include "../../ds/geometry/ray.hpp"

#ifndef CP_ALG_GEOMETRY_RAYS
#define CP_ALG_GEOMETRY_RAYS
namespace cp {
namespace alg {

template <class Tp>
bool is_on(const ds::point<Tp>& p, const ds::ray<Tp>& r) {
    ds::vector<Tp> vec_p(p - r.p);
    if (cross(r.v, vec_p) != 0) return false;
    return dot(r.v, vec_p) >= 0;
}

template <class Tp>
bool is_on(const ds::ray<Tp>& r, const ds::point<Tp>& p) {
    return is_on(p, r);
}

template <class Tp>
bool is_parallel(const ds::ray<Tp>& r1, const ds::ray<Tp>& r2) {
    return cross(r1.v, r2.v) == 0;
}

template <class Tp>
bool is_parallel(const ds::ray<Tp>& r, const ds::segment<Tp>& s) {
    return cross(r.v, ds::vector<Tp>(s.b - s.a)) == 0;
}

template <class Tp>
bool is_parallel(const ds::segment<Tp>& s, const ds::ray<Tp>& r) {
    return is_parallel(r, s);
}

} // namespace alg
} // namespace cp
#endif