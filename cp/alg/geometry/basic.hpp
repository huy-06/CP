
#include <algorithm>
#include "../../ds/geometry/line.hpp"

#ifndef CP_ALG_GEOMETRY_BASIC
#define CP_ALG_GEOMETRY_BASIC
namespace cp {
namespace alg {

template <class Tp>
Tp dot(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return a.x * b.x + a.y * b.y;
}

template <class Tp>
Tp cross(const ds::point<Tp>& a, const ds::point<Tp>& b) {
    return a.x * b.y - a.y * b.x;
}

template <class Tp>
Tp square(const ds::point<Tp>& p) {
    return dot(p, p);
}


template <class Tp>
long double length(const ds::point<Tp>& p) {
    return std::sqrt(square(p));
}


template <class Tp>
ds::point<Tp> normalize(const ds::point<Tp>& p) {
    return p / length(p);
}

template <class Tp>
ds::point<Tp> rotate(const ds::point<Tp>& a) {
    return ds::point<Tp>(-a.y, a.x);
}

template <class Tp>
int sign(const ds::point<Tp>& a) {
    return (a.y > 0 || (a.y == 0 && a.x > 0)) ? 1 : -1;
}

} // namespace alg
} // namespace cp
#endif