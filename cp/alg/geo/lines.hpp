#include "rays.hpp"
#include "../../ds/geometry/line.hpp"

#ifndef CP_ALG_GEOMETRY_LINES
#define CP_ALG_GEOMETRY_LINES
namespace cp {
namespace alg {

template <class Tp>
bool is_on(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return cross(ds::vector<Tp>(p - l.p), l.v) == 0;
}

template <class Tp>
bool is_on(const ds::line<Tp>& l, const ds::point<Tp>& p) {
    return is_on(p, l);
}

template <class Tp>
bool is_on(const ds::segment<Tp>& s, const ds::line<Tp>& l) {
    return is_on(s.a, l) && is_on(s.b, l);
}

template <class Tp>
bool is_on(const ds::line<Tp>& l, const ds::segment<Tp>& s) {
    return is_on(s, l);
}

template <class Tp>
bool is_left(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return cross(l.v, ds::vector<Tp>(p - l.p)) > 0;
}

template <class Tp>
bool is_left(const ds::line<Tp>& l, const ds::point<Tp>& p) {
    return is_left(p, l);
}

template <class Tp>
bool is_left(const ds::segment<Tp>& s, const ds::line<Tp>& l) {
    return is_left(s.a, l) && is_left(s.b, l);
}

template <class Tp>
bool is_left(const ds::line<Tp>& l, const ds::segment<Tp>& s) {
    return is_left(s, l);
}

template <class Tp>
bool is_parallel(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    return cross(l1.v, l2.v) == 0;
}

template <class Tp>
bool is_parallel(const ds::segment<Tp>& s, const ds::line<Tp>& l) {
    return cross(s.b - s.a, l.v) == 0;
}

template <class Tp>
bool is_parallel(const ds::line<Tp>& l, const ds::segment<Tp>& s) {
    return is_parallel(s, l);
}

template <class Tp>
bool is_parallel(const ds::ray<Tp>& r, const ds::line<Tp>& l) {
    return cross(r.v, l.v) == 0;
}

template <class Tp>
bool is_parallel(const ds::line<Tp>& l, const ds::ray<Tp>& r) {
    return is_parallel(r, l);
}

} // namespace alg
} // namespace cp
#endif