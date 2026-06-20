#include "vectors.hpp"
#include "../../ds/geometry/segment.hpp"

#ifndef CP_ALG_GEOMETRY_SEGMENTS
#define CP_ALG_GEOMETRY_SEGMENTS

namespace cp {
namespace alg {

template <class Tp>
Tp length(const ds::segment<Tp>& s) {
    return length(ds::vector<Tp>(s.b - s.a));
}

template <class Tp>
bool is_on(const ds::point<Tp>& p, const ds::segment<Tp>& s) {
    return cross(ds::vector<Tp>(p - s.a), ds::vector<Tp>(s.b - s.a)) == 0
        && dot(ds::vector<Tp>(p - s.a), ds::vector<Tp>(p - s.b)) <= 0;
}

template <class Tp>
bool is_on(const ds::segment<Tp>& s, const ds::point<Tp>& p) {
    return is_on(p, s);
}

template <class Tp>
bool is_left(const ds::point<Tp>& p, const ds::segment<Tp>& s) {
    return cross(ds::vector<Tp>(s.b - s.a), ds::vector<Tp>(p - s.a)) > 0;
}

template <class Tp>
bool is_left(const ds::segment<Tp>& s, const ds::point<Tp>& p) {
    return is_left(p, s);
}

template <class Tp>
bool is_parallel(const ds::segment<Tp>& s1, const ds::segment<Tp>& s2) {
    return cross(ds::vector<Tp>(s1.b - s1.a), ds::vector<Tp>(s2.b - s2.a)) == 0;
}

} // namespace alg
} // namespace cp
#endif