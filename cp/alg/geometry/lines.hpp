#include "basic.hpp"

#ifndef CP_ALG_GEOMETRY_LINES
#define CP_ALG_GEOMETRY_LINES

namespace cp {
namespace alg {

template <class Tp>
long double length(const ds::line<Tp>& l) {
    return length(l.a - l.b);
}

template <class Tp>
bool is_parallel(const ds::line<Tp>& l1, const ds::line<Tp>& l2) {
    return cross(l1.b - l1.a, l2.b - l2.a) == 0;
}

template <class Tp>
bool is_left(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return cross(l.b - l.a, p - l.a) > 0;
}

template <class Tp>
bool on_segment(const ds::point<Tp>& p, const ds::line<Tp>& l) {
    return cross(p - l.a, l.b - l.a) == 0 &&
           std::min(l.a.x, l.b.x) <= p.x && p.x <= std::max(l.a.x, l.b.x) &&
           std::min(l.a.y, l.b.y) <= p.y && p.y <= std::max(l.a.y, l.b.y);
}

} // namespace alg
} // namespace cp
#endif